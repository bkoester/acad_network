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
#include "student.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"
#include "utility.cpp"


using std::cerr; using std::cout; using std::endl;
using std::istream;
using std::istream_iterator;
using std::lock_guard; using std::mutex;
using std::make_pair; using std::move; using std::pair;
using std::max; using std::min;
using std::ref;
using std::set;
using std::thread;
using std::unordered_map;
using std::unordered_set;
using std::vector;
namespace chr = std::chrono;


// By default, use just 1 thread for building networks.
int num_network_building_threads{1};
// output timing information every time we have num_pairs % this variable == 0
const int timing_modulus{10000000};


static unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
GetStudentIdsToCourses(istream& enrollment_stream);

static unordered_map<Course, 
					 unordered_set<Student, StudentHasher>,
					 CourseHasher> 
	GetCoursesToStudents(istream& enrollment_stream);

class StudentNetworkFromStudentPairsBuilder;
static void FindStudentNetworkEdgesFromStudentPairs(
		StudentNetworkFromStudentPairsBuilder& builder);

template <typename StudentsContainer, typename EdgesContainer>
static StudentNetwork PopulateStudentNetwork(
		const StudentsContainer& students, const EdgesContainer& edges);


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
		assert(!course_network.GetEdge(vertex1, vertex2));
		course_network(vertex1, vertex2) = weight;
	}

	return course_network;
}


StudentNetwork BuildStudentNetworkFromEnrollment(
		std::istream& enrollment_stream) {
	using namespace chr;
	auto courses_to_students = GetCoursesToStudents(enrollment_stream);
	auto beginning_pairs_time = system_clock::now();

	// build a map of pairs of students => courses in common
	// build a list of all students
	unordered_set<DistinctUnorderedPair<Student>, 
				  PairHasher<StudentHasher>> edges;
	unordered_set<Student, StudentHasher> students;
	long num_pairs{0};
	for (const auto& course_students_pair : courses_to_students) {
		Course course{course_students_pair.first};
		auto course_students = course_students_pair.second;
		for (auto it1 = course_students.begin(); 
			 it1 != course_students.end(); ++it1) {
			for (auto it2 = it1; ++it2 != course_students.end();) {
				// output debugging information
				if (++num_pairs % timing_modulus == 0) {
					auto duration_in_pairs = duration_cast<seconds>(
							system_clock::now() - beginning_pairs_time);
					cerr << num_pairs << " " << duration_in_pairs.count() 
						 << endl;
				}

				// add course to edge in the graph
				DistinctUnorderedPair<Student> edge{*it1, *it2};
				edges.insert(edge);

				// add the students into the list of students
				students.insert(*it1);
				students.insert(*it2);
			}
		}
	}

	return PopulateStudentNetwork(students, edges);
}


class StudentNetworkFromStudentPairsBuilder {
 public:
	StudentNetworkFromStudentPairsBuilder(const student_container_t& students) : 
		students_(students), it1_{students_.begin()}, it2_{it1_}, num_pairs_{0},
		beginning_pairs_time_{chr::system_clock::now()} {}

	pair<student_container_t::const_iterator, 
		 student_container_t::const_iterator> GetNextIteratorPair()	{
		lock_guard<mutex> iterator_lock_guard{iterator_mutex_};

		// output time information for profiling
		if (++num_pairs_ % timing_modulus == 0) {
			cerr << num_pairs_ << " " << chr::duration_cast<chr::seconds>(
				chr::system_clock::now() - 
				beginning_pairs_time_).count() << endl;
		}

		// return we don't hit the end when incrementing the second iterator
		if (++it2_ != students_.end()) { return make_pair(it1_, it2_); }
		// if it does hit the end, increment the first and restart the second
		it2_ = ++it1_;
		return make_pair(it1_, it2_);
	}

	void AddEdge(StudentId student1, StudentId student2) {
		lock_guard<mutex> edge_lock_guard{edges_mutex_};
		edges_.push_back(make_pair(student1, student2));
	}

	bool FinishedIteration(
			pair<student_container_t::const_iterator, 
				 student_container_t::const_iterator> it_pair) const
	{ return it_pair.first == students_.end(); }

	const vector<pair<StudentId, StudentId>>& edges() { return edges_; }

 private:
	const student_container_t& students_;
	student_container_t::const_iterator it1_;
	student_container_t::const_iterator it2_;
	long num_pairs_;
	chr::time_point<chr::system_clock> beginning_pairs_time_;
	vector<pair<StudentId, StudentId>> edges_;

	mutex iterator_mutex_, edges_mutex_;
};


StudentNetwork BuildStudentNetworkFromStudents(
		const student_container_t& students) {

	// spawn threads to iterate through each pair of students
	StudentNetworkFromStudentPairsBuilder builder{students};
	vector<thread> thread_pool;
	for (int i{0}; i < num_network_building_threads; ++i) {
		thread_pool.push_back(
				thread{FindStudentNetworkEdgesFromStudentPairs, ref(builder)});
	}

	// wait for all threads to complete
	for (auto& t : thread_pool) { t.join(); }

	return PopulateStudentNetwork(students, builder.edges());
}

	
void FindStudentNetworkEdgesFromStudentPairs(
		StudentNetworkFromStudentPairsBuilder& builder) {
	auto it_pair = builder.GetNextIteratorPair();
	while (!builder.FinishedIteration(it_pair)) {
		// Check if the students have taken the same course
		const set<Course>& student1_courses{it_pair.first->courses_taken()};
		const set<Course>& student2_courses{it_pair.second->courses_taken()};
		bool has_intersection{HasIntersection(
				student1_courses.begin(), student1_courses.end(), 
				student2_courses.begin(), student2_courses.end())};

		// Add an edge if the students have taken the same course
		if (has_intersection)
		{ builder.AddEdge(it_pair.first->id(), it_pair.second->id()); }
		it_pair = builder.GetNextIteratorPair();
	}
}


template <typename StudentsContainer, typename EdgesContainer>
StudentNetwork PopulateStudentNetwork(
		const StudentsContainer& students, const EdgesContainer& edges) {
	auto start_network_time = chr::system_clock::now();
	// create the network of students
	StudentNetwork network{students.size()};

	// assign properties of vertices
	auto student_it = students.begin();
	unordered_map<Student, StudentNetwork::vertex_t, StudentHasher> 
		student_to_vertex;
	for (auto vertex_it = network.GetVertices().begin();
			vertex_it != network.GetVertices().end();
			++vertex_it, ++student_it) {
		assert(student_it != students.end());
		student_to_vertex[*student_it] = *vertex_it;
		network[*vertex_it] = student_it->id();
	}

	// add edges and weights
	long edge_num{0};
	for (auto& edge : edges) {
		if (++edge_num % timing_modulus == 0) {
			cerr << edge_num << " " << chr::duration_cast<chr::seconds>(
					chr::system_clock::now() - start_network_time).count()
				 << endl;
		}

		/*auto edge = edge_pair.first;
		auto edge_value = edge_pair.second; */

		StudentNetwork::vertex_t vertex1{student_to_vertex[edge.first]};
		StudentNetwork::vertex_t vertex2{student_to_vertex[edge.second]};

		// INVARIANT: the vertex should not exist in the graph already.
		assert(!network.GetEdge(vertex1, vertex2));
		network(vertex1, vertex2) = 1;
	}

	return network;
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


// Gets a hash table of courses => set of students who have taken it
unordered_map<Course, unordered_set<Student, StudentHasher>, CourseHasher> 
GetCoursesToStudents(istream& enrollment_stream) {
	// set up to read from the enrollment stream
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	unordered_map<Course,
				  unordered_set<Student, StudentHasher>, 
				  CourseHasher> courses_to_students;

	// make a map of courses => students who took them
	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);
		courses_to_students[enrollment.course].insert(
				Student{enrollment.student_id});
	}

	return courses_to_students;
}
