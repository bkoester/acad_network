#ifndef COURSE_NETWORK_H
#define COURSE_NETWORK_H

#include <iosfwd>
#include <unordered_map>

#include <boost/graph/adjacency_list.hpp>

#include "course.hpp"

class CourseNetwork {
 public:
	CourseNetwork(std::istream& input);

	using course_graph_t = boost::adjacency_list<boost::vecS, boost::vecS, 
		  boost::undirectedS, Course, int>;

	// classes declared to provide iterator access to vertices
	// constructors are private to only allow access through GetVertices() and
	// GetEdges() functions, which supply the graph parameter to the constructors
	class Vertices {
	 public:
		auto begin() { return boost::vertices(graph_).first; }

		auto end() { return boost::vertices(graph_).second; }

	 private:
		friend class CourseNetwork;
		Vertices(course_graph_t& graph) : graph_{graph} {}

		course_graph_t& graph_;
	};

	class Edges {
	 public:
		auto begin() { return boost::edges(graph_).first; }

		auto end() { return boost::edges(graph_).second; }

	 private:
		friend class CourseNetwork;
		Edges(course_graph_t& graph) : graph_{graph} {}

		course_graph_t& graph_;
	};


	Vertices GetVertices() { return Vertices{graph_}; }
	Edges GetEdges() { return Edges{graph_}; }
	

 private:
	using vertex_t = boost::graph_traits<course_graph_t>::vertex_descriptor;
	using edge_t = boost::graph_traits<course_graph_t>::edge_descriptor;

	void AddVertex(
			std::unordered_map<Course, vertex_t, CourseHasher>& course_to_vertex,
			const Course& course);
	
	course_graph_t graph_; 
	std::unordered_map<Course, vertex_t, CourseHasher> course_to_vertex_;
};

#endif  // COURSE_NETWORK_H
