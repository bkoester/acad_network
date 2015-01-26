#include "graph_builder.hpp"

#include <cassert>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "course.hpp"
#include "course_tab.hpp"


using std::getline; using std::string;
using std::max; using std::min;
using std::istream;
using std::pair;
using std::unordered_map;
using std::unordered_set;


static unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
GetStudentIdsToCourses(istream& course_tab_stream);


static unordered_map<Course, 
					 unordered_set<StudentId, StudentIdHasher>,
					 CourseHasher> 
	GetCoursesToStudentIds(istream& course_tab_stream);


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
	int operator()(const DistinctUnorderedPair<T>& pair) const
	{ return hasher(pair.first) ^ hasher(pair.second); }

	H hasher;
};


CourseNetwork BuildCourseGraphFromTab(istream& course_tab_stream) {
	auto student_to_courses = GetStudentIdsToCourses(course_tab_stream);

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


StudentNetwork BuildStudentGraphFromTab(std::istream& course_tab_stream) {
	auto courses_to_students = GetCoursesToStudentIds(course_tab_stream);

	// build a map of pairs of students => courses in common
	// build a list of all students
	unordered_set<DistinctUnorderedPair<StudentId>, 
				  PairHasher<StudentIdHasher>> edges;
	unordered_set<StudentId, StudentIdHasher> students;
	for (const auto& course_students_pair : courses_to_students) {
		Course course{course_students_pair.first};
		auto course_students = course_students_pair.second;
		for (auto it1 = course_students.begin(); 
			 it1 != course_students.end(); ++it1) {
			for (auto it2 = it1; ++it2 != course_students.end();) {
				// add course to edge in the graph
				DistinctUnorderedPair<StudentId> edge{*it1, *it2};
				edges.insert(edge);

				// add the students into the list of students
				students.insert(*it1);
				students.insert(*it2);
			}
		}
	}

	// create new graph with the number of students as number of vertices
	StudentNetwork network{students.size()};

	// assign properties of vertices
	auto student_it = students.begin();
	unordered_map<StudentId, StudentNetwork::vertex_t, StudentIdHasher> 
		student_to_vertex;
	for (auto vertex_it = network.GetVertices().begin();
			vertex_it != network.GetVertices().end();
			++vertex_it, ++student_it) {
		assert(student_it != students.end());
		student_to_vertex[*student_it] = *vertex_it;
		network[*vertex_it] = *student_it;
	}

	// add edges and weights
	for (auto& edge : edges) {
		/*
		auto edge = edge_pair.first;
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
GetStudentIdsToCourses(istream& course_tab_stream) {
	CourseTab course_tab{course_tab_stream};
	unordered_map<StudentId, unordered_set<Course, CourseHasher>> 
		student_to_courses;

	// fill in the course information
	for (const CourseTab::Line& line : course_tab)
	{ student_to_courses[line.student].insert(line.course); }

	return student_to_courses;
}


// Gets a hash table of courses => set of students who have taken it
unordered_map<Course, unordered_set<StudentId, StudentIdHasher>, CourseHasher> 
GetCoursesToStudentIds(istream& course_tab_stream) {
	CourseTab course_tab{course_tab_stream};
	unordered_map<Course,
				  unordered_set<StudentId, StudentIdHasher>, 
				  CourseHasher> courses_to_students;

	// fill in the course information
	for (const CourseTab::Line& line : course_tab)
	{ courses_to_students[line.course].insert(line.student); }

	return courses_to_students;
}
