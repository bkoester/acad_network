#include "graph_builder.hpp"

#include <cassert>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <numeric>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/optional.hpp>

#include "course.hpp"
#include "course_container.hpp"
#include "course_network.hpp"
#include "mem_usage.hpp"
#include "student.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"
#include "weighting_function.hpp"


using std::begin; using std::end; using std::istream_iterator;
using std::cerr; using std::cout; using std::endl;
using std::cref; using std::ref; using std::bind; using std::placeholders::_1;
using std::istream;
using std::lock_guard; using std::mutex;
using std::make_pair; using std::pair;
using std::set;
using std::thread;
using std::unordered_map;
using std::unordered_set;
using std::vector;
namespace chr = std::chrono;

using boost::optional;

using weighting_func_ptr = optional<double>(*)(const Student&, const Student&);


// output timing information every time we have num_pairs % this variable == 0
const int timing_modulus{10000000};


static unordered_map<Student::Id, unordered_set<Course::Id, Course::Id::Hasher>> 
GetStudentIdsToCourses(const StudentContainer& students);

class StudentNetworkBuilder;
void CalculateStudentNetworkEdges(const StudentNetwork& network,
		const StudentContainer& students,
		StudentNetworkBuilder& builder,
		weighting_func_ptr weighting_func);


template <typename T>
struct DistinctUnorderedPair {
	T first, second;

	DistinctUnorderedPair(T first_, T second_) :
		first{first_}, second{second_} { assert(first != second); }

	bool operator==(const DistinctUnorderedPair<T>& other) const {
		return (other.first == first && other.second == second) ||
			(other.first == second && other.second == first);
	}
};


template <typename H>
struct PairHasher {
	template <typename T>
	int operator()(const T& pair) const
	{ return hasher(pair.first) ^ hasher(pair.second); }

	H hasher;
};


CourseNetwork BuildCourseNetworkFromEnrollment(
		const StudentContainer& students) {
	auto student_to_courses = GetStudentIdsToCourses(students);

	// aggregate pairs of courses
	unordered_map<DistinctUnorderedPair<Course::Id>,
				  int, PairHasher<Course::Id::Hasher>> edge_weights;
	unordered_set<Course::Id, Course::Id::Hasher> courses;
	for (const auto& elt : student_to_courses) {
		if (elt.second.size() == 1) { courses.insert(*elt.second.begin()); }

		// get all pairs of courses
		for (auto it = elt.second.cbegin(); it != elt.second.cend(); ++it) {
			for (auto it2 = it; ++it2 != elt.second.cend();) {
				DistinctUnorderedPair<Course::Id> e{*it, *it2};
				++edge_weights[e];

				// create the two courses
				courses.insert(*it);
				courses.insert(*it2);
			}
		}
	}

	// create new graph so all vertices can be added at once
	CourseNetwork course_network{begin(courses), end(courses)};

	// add edges and weights
	for (auto& edge_pair : edge_weights) {
		auto e = edge_pair.first;
		int weight{edge_pair.second};

		CourseNetwork::vertex_t vertex1{
			course_network.GetVertex(e.first)};
		CourseNetwork::vertex_t vertex2{
			course_network.GetVertex(e.second)};

		// INVARIANT: the edge should not exist in the graph already.
		assert(!course_network.GetEdgeDescriptor(vertex1, vertex2));
		course_network(vertex1, vertex2) = weight;
	}

	return course_network;
}


class StudentNetworkBuilder {
 public:
	StudentNetworkBuilder(
			StudentNetwork& network, const StudentContainer& students) :
				network_(network), num_pairs_{0},
				beginning_pairs_time_{chr::system_clock::now()} {
		// assign the vertices in the network
		auto student_it = begin(students);
		for (auto vertex_it = network_.GetVertexValues().begin();
				vertex_it != network_.GetVertexValues().end();
				++vertex_it, ++student_it) {
			assert(student_it != students.end());
			*vertex_it = student_it->id();
		}

		it1_ = begin(network_.GetVertexDescriptors());
		it2_ = begin(network_.GetVertexDescriptors());
	}

	pair<StudentNetwork::vertex_descriptors_t::iterator_t,
		 StudentNetwork::vertex_descriptors_t::iterator_t>
	GetNextIteratorPair() {
		lock_guard<mutex> iterator_lock_guard{iterator_mutex_};

		// output time information for profiling
		if (++num_pairs_ % timing_modulus == 0) {
			cerr << num_pairs_ << " " << chr::duration_cast<chr::seconds>(
				chr::system_clock::now() -
				beginning_pairs_time_).count() << endl;
			cerr << "Mem usage: " << GetMemoryUsage() << endl;
		}

		// Return if we don't hit the end when incrementing the second iterator.
		if (!ReachedEndOfStudents(++it2_)) { return make_pair(it1_, it2_); }

		// if it does hit the end, increment the first and restart the second
		it2_ = ++it1_;
		return make_pair(it1_, it2_);
	}

	void AddEdge(StudentNetwork::vertex_t student1,
			StudentNetwork::vertex_t student2, double value) {
		lock_guard<mutex> edge_lock_guard{edges_mutex_};
		network_(student1, student2) = value;
	}

	bool ReachedEndOfStudents(
			StudentNetwork::vertex_descriptors_t::iterator_t it) const
	{ return it == end(network_.GetVertexDescriptors()); }

 private:
	StudentNetwork& network_;
	StudentNetwork::vertex_descriptors_t::iterator_t it1_, it2_;
	long num_pairs_;
	chr::time_point<chr::system_clock> beginning_pairs_time_;

	mutex iterator_mutex_, edges_mutex_;
};


StudentNetwork BuildStudentNetworkFromStudents(
		const StudentContainer& students, weighting_func_ptr weighting_func) {

	// spawn threads to iterate through each pair of students
	StudentNetwork network{students.size()};
	StudentNetworkBuilder builder{network, students};
	vector<thread> thread_pool;
	for (int i{0}; i < num_threads; ++i) {
		thread_pool.emplace_back(CalculateStudentNetworkEdges, cref(network),
				cref(students), ref(builder), weighting_func);
	}

	// wait for all threads to complete
	for_each(begin(thread_pool), end(thread_pool), bind(&thread::join, _1));

	return network;
}


void CalculateStudentNetworkEdges(const StudentNetwork& network,
								  const StudentContainer& students,
								  StudentNetworkBuilder& builder,
								  weighting_func_ptr weighting_func) {
	for (auto it_pair = builder.GetNextIteratorPair();
			!builder.ReachedEndOfStudents(it_pair.first);
			it_pair = builder.GetNextIteratorPair()) {
		if (it_pair.first == it_pair.second) { continue; }
		// Get the courses each of the students have taken.
		const Student& student1(students.Find(network[*it_pair.first]));
		const Student& student2(students.Find(network[*it_pair.second]));

		if (auto connection = weighting_func(student1, student2)) {
			builder.AddEdge(
					*it_pair.first, *it_pair.second, connection.value());
		}
	}
}


// Gets a hash table of students => set of courses they have taken
unordered_map<Student::Id, unordered_set<Course::Id, Course::Id::Hasher>>
GetStudentIdsToCourses(const StudentContainer& students) {
	unordered_map<Student::Id, unordered_set<Course::Id, Course::Id::Hasher>>
		student_to_courses;

	// make a map of student id => courses taken
	for (const auto& student : students) {
		for (const auto& course : student.courses_taken()) {
			student_to_courses[student.id()].insert(course->GetId());
		}
	}

	return student_to_courses;
}
