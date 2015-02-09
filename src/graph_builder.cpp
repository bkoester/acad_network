#include "graph_builder.hpp"

#include <cassert>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "course.hpp"
#include "course_network.hpp"
#include "mem_usage.hpp"
#include "student.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


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


// output timing information every time we have num_pairs % this variable == 0
const int timing_modulus{10000000};


static unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
GetStudentIdsToCourses(istream& enrollment_stream);

class StudentNetworkBuilder;
void CalculateStudentNetworkEdges(const StudentNetwork& network,
		const student_container_t& students,
		StudentNetworkBuilder& builder);


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


CourseNetwork BuildCourseNetworkFromEnrollment(istream& enrollment_stream) {
	auto student_to_courses = GetStudentIdsToCourses(enrollment_stream);

	// aggregate pairs of courses
	unordered_map<DistinctUnorderedPair<Course>, int, PairHasher<CourseHasher>> 
		edge_weights;
	unordered_set<Course, CourseHasher> courses;
	for (const auto& elt : student_to_courses) {
		if (elt.second.size() == 1) { courses.insert(*elt.second.begin()); }

		// get all pairs of courses
		for (auto it = elt.second.cbegin(); it != elt.second.cend(); ++it) {
			for (auto it2 = it; ++it2 != elt.second.cend();) {
				DistinctUnorderedPair<Course> e{*it, *it2};
				++edge_weights[e];

				// create the two courses
				courses.insert(*it);
				courses.insert(*it2);
			}
		}
	}

	// create new graph so all vertices can be added at once
	CourseNetwork course_network{courses.begin(), courses.end()};

	// add edges and weights
	for (auto& edge_pair : edge_weights) {
		auto e = edge_pair.first;
		int weight{edge_pair.second};

		CourseNetwork::vertex_t vertex1{course_network.GetVertex(e.first)};
		CourseNetwork::vertex_t vertex2{course_network.GetVertex(e.second)};

		// INVARIANT: the edge should not exist in the graph already.
		assert(!course_network.GetEdgeDescriptor(vertex1, vertex2));
		course_network(vertex1, vertex2) = weight;
	}

	return course_network;
}


class StudentNetworkBuilder {
 public:
	StudentNetworkBuilder(
			StudentNetwork& network, const student_container_t& students) :
				network_(network), num_pairs_{0},
				beginning_pairs_time_{chr::system_clock::now()} {
		// assign the vertices in the network
		auto student_it = students.begin();
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
		const student_container_t& students) {

	// spawn threads to iterate through each pair of students
	StudentNetwork network{students.size()};
	StudentNetworkBuilder builder{network, students};
	vector<thread> thread_pool;
	for (int i{0}; i < num_threads; ++i) {
		thread_pool.emplace_back(CalculateStudentNetworkEdges, cref(network), 
				cref(students), ref(builder));
	}

	// wait for all threads to complete
	for_each(begin(thread_pool), end(thread_pool), bind(&thread::join, _1));

	return network;
}

	
void CalculateStudentNetworkEdges(const StudentNetwork& network,
		const student_container_t& students,
		StudentNetworkBuilder& builder) {
	auto it_pair = builder.GetNextIteratorPair();
	while (!builder.ReachedEndOfStudents(it_pair.first)) {
		// Get the courses each of the students have taken.
		const Student& student1(*lower_bound(begin(students), end(students), 
				network[*it_pair.first]));
		const Student& student2(*lower_bound(begin(students), end(students), 
				network[*it_pair.second]));
		const set<const Course*>& student1_courses(student1.courses_taken());
		const set<const Course*>& student2_courses(student2.courses_taken());

		// Create a list of the courses the students have in common.
		vector<const Course*> courses_in_common;
		set_intersection(begin(student1_courses), end(student1_courses),
						 begin(student2_courses), end(student2_courses),
						 back_inserter(courses_in_common));

		// Calculate the connection between the two students.
		double connection{accumulate(begin(courses_in_common), 
				end(courses_in_common), 0.,
				[](double connection, const Course* course) { 
					return connection + course->num_credits() / 
						double(course->GetNumStudentsEnrolled());
				})};

		// Add an edge with the appropriate connection value.
		if (!courses_in_common.empty()) {
			builder.AddEdge(*it_pair.first, *it_pair.second, connection); 
		}
		it_pair = builder.GetNextIteratorPair();
	}
}


// Gets a hash table of students => set of courses they have taken
unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
GetStudentIdsToCourses(istream& enrollment_stream) {
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
		student_to_courses;

	// make a map of student id => courses taken
	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);
		student_to_courses[enrollment.student_id].insert(enrollment.course);
	}
	
	return student_to_courses;
}
