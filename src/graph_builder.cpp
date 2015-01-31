#include "graph_builder.hpp"

#include <cassert>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "course.hpp"
#include "course_network.hpp"
#include "student.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"
#include "utility.cpp"


using std::cerr; using std::endl;
using std::getline; using std::string;
using std::istream;
using std::istream_iterator;
using std::make_pair; using std::pair;
using std::max; using std::min;
using std::set;
using std::set_intersection;
using std::unordered_map;
using std::unordered_set;


static unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
GetStudentIdsToCourses(istream& enrollment_stream);

static unordered_map<Course, 
					 unordered_set<Student, StudentHasher>,
					 CourseHasher> 
	GetCoursesToStudents(istream& enrollment_stream);

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
	using namespace std::chrono;
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
				if (++num_pairs % 10000 == 0) {
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


StudentNetwork BuildStudentNetworkFromStudents(
		const student_container_t& students) {
	using namespace std::chrono;
	long num_pairs{0};
	auto beginning_pairs_time = system_clock::now();

	unordered_set<pair<StudentId, StudentId>, PairHasher<StudentIdHasher>> edges;
	for (auto it1 = students.begin(); it1 != students.end(); ++it1) {
		for (auto it2 = it1; ++it2 != students.end();) {
			// Check if the students have taken the same course
			const set<Course>& student1_courses{it1->courses_taken()};
			const set<Course>& student2_courses{it2->courses_taken()};
			bool has_intersection{HasIntersection(
					student1_courses.begin(), student1_courses.end(), 
					student2_courses.begin(), student2_courses.end())};

			// Add an edge if the students have taken the same course
			if (has_intersection)
			{ edges.insert(make_pair(it1->id(), it2->id())); }

			if (++num_pairs % 10000 == 0) {
				cerr << num_pairs << " " << duration_cast<seconds>(
					system_clock::now() - beginning_pairs_time).count() << endl;
			}
		}
	}

	return PopulateStudentNetwork(students, edges);
}


template <typename StudentsContainer, typename EdgesContainer>
StudentNetwork PopulateStudentNetwork(
		const StudentsContainer& students, const EdgesContainer& edges) {
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
	for (auto& edge : edges) {
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
		const Enrollment& enrollment{*enrollment_it};
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
		const Enrollment& enrollment{*enrollment_it};
		courses_to_students[enrollment.course].insert(
				Student{enrollment.student_id});
	}

	return courses_to_students;
}
