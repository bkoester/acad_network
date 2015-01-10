#ifndef NETWORK_H
#define NETWORK_H

#include <unordered_map>
#include <iostream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/optional.hpp>

#include "course.hpp"

class NoEdgeException{};

// Define abstract templated class for Network.
template <typename Vertex, typename Edge>
class Network {
 public:
	// construct empty graph
	Network();
	// ifstream must contain a boost input archive.
	Network(std::ifstream& input);

	// Copy constructors only copy the graph_ member. vertices_ and edges_ will
	// still reference the same graph_ object and do not need to be copied.
	Network(const Network& other);
	Network& operator=(const Network& other);
	// No move operations defined because the graph is allocated on the stack
	// and it is the most memory intensive member, so there's no point in
	// defining a move operation for the other members' sake. The copy fallback
	// will do.

	virtual ~Network() {}

	using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, 
		  boost::undirectedS, Vertex, Edge>;
	using vertex_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
	using edge_t = typename boost::graph_traits<graph_t>::edge_descriptor;

	// gets the vertex type in the graph for a specific course
	// will throw out_of_range exception for the vertex
	vertex_t GetVertex(const Course& course) const;

	vertex_t GetSourceVertex(const edge_t& edge) const;
	vertex_t GetTargetVertex(const edge_t& edge) const;
	boost::optional<edge_t> GetEdge(
			const vertex_t& source, const vertex_t& target) const;

	Edge& operator[](const edge_t& edge) { return graph_[edge]; }
	const Edge& operator[](const edge_t& edge) const { return graph_[edge]; }

	Vertex& operator[](const vertex_t& vertex) { return graph_[vertex]; }
	const Vertex& operator[](const vertex_t& vertex) const
	{ return graph_[vertex]; }

	virtual Edge operator()(
			const vertex_t& source, const vertex_t& target) const;

	void Save(std::ostream& os);
	void Load(std::istream& is);

	// classes declared to provide iterator access to vertices
	// constructors are private to require access through GetVertices() and
	// GetEdges() functions, which supply the graph parameter to the constructors
	class Vertices {
	 public:
		auto begin() { return boost::vertices(graph_).first; }
		const auto begin() const { return boost::vertices(graph_).first; }
		const auto cbegin() const { return boost::vertices(graph_).first; }

		auto end() { return boost::vertices(graph_).second; }
		const auto end() const { return boost::vertices(graph_).first; }
		const auto cend() const { return boost::vertices(graph_).first; }

	 private:
		friend class Network;
		Vertices(graph_t& graph) : graph_{graph} {}

		graph_t& graph_;
	};

	class Edges {
	 public:
		auto begin() { return boost::edges(graph_).first; }
		const auto begin() const { return boost::edges(graph_).first; }
		const auto cbegin() const { return boost::edges(graph_).first; }

		auto end() { return boost::edges(graph_).second; }
		const auto end() const { return boost::edges(graph_).second; }
		const auto cend() const { return boost::edges(graph_).second; }

	 private:
		friend class Network;
		Edges(graph_t& graph) : graph_{graph} {}

		graph_t& graph_;
	};

	Vertices GetVertices() { return vertices_; }
	Vertices GetVertices() const { return vertices_; }
	Edges GetEdges() { return edges_; }
	Edges GetEdges() const { return edges_; }

 protected:
	// It's usually considered bad design to have protected members, but derived
	// classes need more access to this than external code, and the BGL provides
	// its own encapsulation.
	graph_t graph_;  

 private:
	Vertices vertices_;
	Edges edges_;
};


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network() : vertices_{graph_}, edges_{graph_} {}


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(std::ifstream& input) : 
		vertices_{graph_}, edges_{graph_} { Load(input); }


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(const Network& other) : 
	graph_{other.graph_}, vertices_{graph_}, edges_{graph_} {}


template <typename Vertex, typename Edge>
Network<Vertex, Edge>& Network<Vertex, Edge>::operator=(const Network& other) {
	graph_ = other.graph_; 
	return *this;
}

template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t Network<Vertex, Edge>::GetSourceVertex(
		const edge_t& edge) const { return source(edge, graph_); }


template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t Network<Vertex, Edge>::GetTargetVertex(
		const edge_t& edge) const { return target(edge, graph_); }


template <typename Vertex, typename Edge>
boost::optional<typename Network<Vertex, Edge>::edge_t> 
Network<Vertex, Edge>::GetEdge(
		const vertex_t& source, const vertex_t& target) const { 
	auto boost_edge = boost::edge(source, target, graph_);
	return boost::make_optional(boost_edge.second, boost_edge.first); 
}


template <typename Vertex, typename Edge>
Edge Network<Vertex, Edge>::operator()(
		const vertex_t& source, const vertex_t& target) const {
	boost::optional<edge_t> edge{GetEdge(source, target)};
	if (!edge) { throw NoEdgeException{}; };
	return operator[](edge.get());
}


template <typename Vertex, typename Edge>
void Network<Vertex, Edge>::Save(std::ostream& output) {
	// create boost archive from ostream and save the graph
	boost::archive::text_oarchive archive{output};
	archive << graph_;
}


template <typename Vertex, typename Edge>
void Network<Vertex, Edge>::Load(std::istream& input) {
	boost::archive::text_iarchive archive{input};
	archive >> graph_;
}


#endif  // NETWORK_H
