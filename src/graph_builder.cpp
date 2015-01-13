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


struct CoursePair {
	Course vertex1, vertex2;
	CoursePair(Course course1, Course course2) :
		vertex1{min(course1, course2)}, vertex2{max(course1, course2)}
	{ assert(vertex1 != vertex2); }

	bool operator==(const CoursePair& other) const {
		return (other.vertex1 == vertex1 && other.vertex2 == vertex2) ||
			(other.vertex1 == vertex2 && other.vertex2 == vertex1);
	}
};


struct CoursePairHasher {
	int operator()(const CoursePair& edge) const
	{ return hasher(edge.vertex1) ^ hasher(edge.vertex2); }

	CourseHasher hasher;
};


CourseNetwork::graph_t BuildCourseGraphFromTab(istream& course_tab_stream) {
	// loop over all students
	CourseTab course_tab{course_tab_stream};
	unordered_map<int, unordered_set<Course, CourseHasher>> student_to_courses;
	for (const CourseTab::Line& line : course_tab) {
		// fill in the course information
		student_to_courses[line.student.id()].insert(line.course);
	}

	// aggregate pairs of courses
	unordered_map<CoursePair, int, CoursePairHasher> edge_weights;
	unordered_set<Course, CourseHasher> courses;
	for (const auto& elt : student_to_courses) {
		if (elt.second.size() == 1) { courses.insert(*elt.second.begin()); }

		// get all pairs of courses
		for (auto it = elt.second.cbegin(); it != elt.second.cend(); ++it) {
			for (auto it2 = it; ++it2 != elt.second.cend();) {
				CoursePair e{*it, *it2};
				++edge_weights[e];

				// create the two courses
				courses.insert(*it);
				courses.insert(*it2);
			}
		}
	}

	// create new graph so all vertices can be added at once
	CourseNetwork::graph_t graph{courses.size()};

	// assign properties of vertices
	unordered_map<Course, CourseNetwork::vertex_t, CourseHasher> 
		course_to_vertex;
	auto course_it = courses.begin();
	for (auto vertex_it = vertices(graph).first; 
			vertex_it != vertices(graph).second; ++vertex_it) {
		assert(course_it != courses.end());
		course_to_vertex[*course_it] = *vertex_it;
		graph[*vertex_it] = *course_it++;
	}

	// add edges and weights
	for (auto& edge_pair : edge_weights) {
		CoursePair e{edge_pair.first};
		int weight{edge_pair.second};

		CourseNetwork::vertex_t vertex1{course_to_vertex[e.vertex1]};
		CourseNetwork::vertex_t vertex2{course_to_vertex[e.vertex2]};

		// INVARIANT: the vertex should not exist in the graph already.
		assert(!edge(vertex1, vertex2, graph).second);
		CourseNetwork::edge_t edge{add_edge(
				vertex1, vertex2, weight, graph).first};
	}

	return graph;
}
