#include "graph_builder.hpp"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <boost/graph/adjacency_list.hpp>

#include "course.hpp"
#include "course_tab.hpp"


using std::cerr; using std::endl;
using std::getline; using std::string;
using std::max; using std::min;
using std::istream;
using std::pair;
using std::stringstream;
using std::unordered_map;
using std::unordered_set;

using boost::vertices;
using boost::add_edge;
using boost::source;


static unordered_map<Student, unordered_set<Course, CourseHasher>, StudentHasher> 
GetStudentsToCourses(istream& course_tab_stream);


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
	auto student_to_courses = GetStudentsToCourses(course_tab_stream);

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


StudentNetwork::graph_t BuildStudentGraphFromTab(
		std::istream& course_tab_stream) {
	auto student_to_courses = GetStudentsToCourses(course_tab_stream);

	// get pairs of students and courses they have in common
	unordered_map<DistinctUnorderedPair<Student>, int, PairHasher<StudentHasher>> 
		edge_weights;
	for (auto it1 = student_to_courses.cbegin(); 
			it1 != student_to_courses.cend(); ++it1) {
		for (auto it2 = it1; 
				it2 != student_to_courses.cend(); ++it2) {
			DistinctUnorderedPair<Student> edge{it1->first, it2->first};
			++edge_weights[edge];
		}
	}

	// create new graph with the number of students as number of vertices
	StudentNetwork::graph_t graph{student_to_courses.size()};

	// assign properties of vertices
	auto student_it = student_to_courses.begin();
	unordered_map<Student, StudentNetwork::vertex_t, StudentHasher> 
		student_to_vertex;
	for (auto vertex_it = vertices(graph).first; 
			vertex_it != vertices(graph).second; ++vertex_it, ++student_it) {
		assert(student_it != student_to_courses.end());
		student_to_vertex[student_it->first] = *vertex_it;
		graph[*vertex_it] = student_it->first;
	}

	// add edges and weights
	for (auto& edge_pair : edge_weights) {
		auto e = edge_pair.first;
		int weight{edge_pair.second};

		StudentNetwork::vertex_t vertex1{student_to_vertex[e.first]};
		StudentNetwork::vertex_t vertex2{student_to_vertex[e.second]};

		// INVARIANT: the vertex should not exist in the graph already.
		assert(!edge(vertex1, vertex2, graph).second);
		StudentNetwork::edge_t edge{add_edge(
				vertex1, vertex2, weight, graph).first};
	}

	return graph;
}


// Gets a hash table of students => set of courses they have taken
unordered_map<Student, unordered_set<Course, CourseHasher>, StudentHasher> 
GetStudentsToCourses(istream& course_tab_stream) {
	CourseTab course_tab{course_tab_stream};
	unordered_map<Student, unordered_set<Course, CourseHasher>, StudentHasher> 
		student_to_courses;

	// fill in the course information
	for (const CourseTab::Line& line : course_tab)
	{ student_to_courses[line.student].insert(line.course); }

	return student_to_courses;
}
