#ifndef COURSE_NETWORK_H
#define COURSE_NETWORK_H

#include <iosfwd>
#include <memory>
#include <unordered_map>

#include <boost/graph/adjacency_list.hpp>

#include "course.hpp"

class CourseNetwork {
 public:
	CourseNetwork(std::istream& input);

	using course_graph_t = boost::adjacency_list<boost::vecS, boost::vecS, 
		  boost::undirectedS, Course, int>;
	using vertex_t = boost::graph_traits<course_graph_t>::vertex_descriptor;
	using edge_t = boost::graph_traits<course_graph_t>::edge_descriptor;

	// gets the vertex type in the graph for a specific course
	// will throw out_of_range exception for the vertex
	vertex_t GetVertex(const Course& course);
	const vertex_t GetVertex(const Course& course) const;

	Course GetSource(const edge_t& edge) const;
	Course GetTarget(const edge_t& edge) const;
	int edge(const vertex_t& source, const vertex_t& target) const;

	int operator[](const edge_t& edge) const { return graph_[edge]; }

	// classes declared to provide iterator access to vertices
	// constructors are private to require access through GetVertices() and
	// GetEdges() functions, which supply the graph parameter to the constructors
	class Vertices {
	 public:
		auto begin() { return boost::vertices(graph_).first; }
		const auto begin() const { return boost::vertices(graph_).first; }

		auto end() { return boost::vertices(graph_).second; }
		const auto end() const { return boost::vertices(graph_).first; }

	 private:
		friend class CourseNetwork;
		Vertices(course_graph_t& graph) : graph_{graph} {}

		course_graph_t& graph_;
	};

	class Edges {
	 public:
		auto begin() { return boost::edges(*graph_).first; }
		const auto begin() const { return boost::edges(*const_graph_).first; }

		auto end() { return boost::edges(*graph_).second; }
		const auto end() const { return boost::edges(*const_graph_).first; }

	 private:
		friend class CourseNetwork;
		Edges(course_graph_t& graph) : graph_{&graph}, const_graph_{nullptr} {}
		Edges(const course_graph_t& graph) : 
			graph_{nullptr}, const_graph_{&graph} {}

		course_graph_t* graph_;
		const course_graph_t* const_graph_;
	};

	Vertices GetVertices() { return Vertices{graph_}; }
	Edges GetEdges() { return Edges{graph_}; }
	const Edges GetEdges() const { return Edges{graph_}; }


 private:
	void AddVertex(
			std::unordered_map<Course, vertex_t, CourseHasher>& course_to_vertex,
			const Course& course);
	
	course_graph_t graph_; 
	std::unordered_map<Course, vertex_t, CourseHasher> course_to_vertex_;
};

std::ostream& operator<<(
		std::ostream& output, const CourseNetwork& course_network);

#endif  // COURSE_NETWORK_H
