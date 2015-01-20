#ifndef NETWORK_H
#define NETWORK_H

#include <unordered_map>
#include <iostream>
#include <iterator>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/optional.hpp>

class NoEdgeException{};

// Define abstract templated class for Network.
template <typename Vertex, typename Edge>
class Network {
 public:
	// convenient for dealing with graphs
	using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, 
		  boost::undirectedS, Vertex, Edge>;
	using vertex_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
	using edge_t = typename boost::graph_traits<graph_t>::edge_descriptor;
	using degree_t = typename boost::graph_traits<graph_t>::degree_size_type;

	// Construct empty graph.
	Network();
	// ifstream must contain a boost input archive.
	Network(std::istream& input);
	// Create a network with the given number of vertices.
	Network(long unsigned int num_vertices);
	// Construct a network with a certain starting set of vertices given in the
	// range by the iterators.
	template <typename ForwardIt>
	Network(ForwardIt start, ForwardIt end);
	// Allows a network to be built from a non-boost archive graph format.
	Network(const graph_t& graph);

	// Copy constructors only copy the graph_ member. vertices_ and edges_ will
	// still reference the same graph_ object and do not need to be copied.
	Network(const Network& other);
	Network& operator=(const Network& other);
	// No move operations defined because the graph is allocated on the stack
	// and it is the most memory intensive member, so there's no point in
	// defining a move operation for the other members' sake. The copy fallback
	// will do.

	virtual ~Network() {}

	degree_t Degree(const vertex_t& vertex) 
	{ return boost::degree(vertex, graph_); }
	
	vertex_t GetSourceVertexDescriptor(const edge_t& edge) const;
	vertex_t GetTargetVertexDescriptor(const edge_t& edge) const;
	boost::optional<edge_t> GetEdge(
			const vertex_t& source, const vertex_t& target) const;

	auto GetEdgeIterators(const vertex_t& vertex)
	{ return boost::in_edges(vertex, graph_); }

	Edge& operator[](const edge_t& edge) { return graph_[edge]; }
	const Edge& operator[](const edge_t& edge) const { return graph_[edge]; }

	Vertex& operator[](const vertex_t& vertex) { return graph_[vertex]; }
	const Vertex& operator[](const vertex_t& vertex) const
	{ return graph_[vertex]; }

	// The design of throwing error vs. creating edge is modeled after the
	// member function calls of (unordered_)map.
	// Returns the data structure at the edge, throws error if it doesn't exist.
	const Edge& Get(const vertex_t& source, const vertex_t& target) const;
	Edge& Get(const vertex_t& source, const vertex_t& target);

	// Returns the data structure at the edge, creates edge if it doesn't exist.
	const Edge& operator()(const vertex_t& source, const vertex_t& target) const;
	Edge& operator()(const vertex_t& source, const vertex_t& target);

	void Save(std::ostream& output_graph_archive) const;
	void Load(std::istream& input_graph_archive);

	// classes declared to provide iterator access to vertices
	// constructors are private to require access through GetVertices() and
	// GetEdges() functions, which supply the graph parameter to the constructors
	class Vertices {
	 public:
		auto size() const { return boost::num_vertices(graph_); }
		auto begin() { return boost::vertices(graph_).first; }
		const auto begin() const { return boost::vertices(graph_).first; }
		const auto cbegin() const { return boost::vertices(graph_).first; }

		auto end() { return boost::vertices(graph_).second; }
		const auto end() const { return boost::vertices(graph_).second; }
		const auto cend() const { return boost::vertices(graph_).second; }

	 private:
		friend class Network;
		Vertices(graph_t& graph) : graph_{graph} {}

		graph_t& graph_;
	};

	class Edges {
	 public:
		auto size() const { return boost::num_edges(graph_); }
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
Network<Vertex, Edge>::Network(long unsigned int num_vertices) : 
	graph_{num_vertices}, vertices_{graph_}, edges_{graph_} {}


template <typename Vertex, typename Edge>
template <typename ForwardIt>
Network<Vertex, Edge>::Network(ForwardIt first, ForwardIt last) : 
		graph_{std::distance(first, last)}, vertices_{graph_}, edges_{graph_} {
	auto it = first;
	for (auto& vertex : GetVertices()) { operator[](vertex) = *it++; }
}

template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(std::istream& input) : 
		vertices_{graph_}, edges_{graph_} { Load(input); }


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(const graph_t& graph) :
	graph_{graph}, vertices_{graph_}, edges_{graph_} {}


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(const Network& other) : 
	graph_{other.graph_}, vertices_{graph_}, edges_{graph_} {}


template <typename Vertex, typename Edge>
Network<Vertex, Edge>& Network<Vertex, Edge>::operator=(const Network& other) {
	// using the copy-swap idiom
	graph_t graph{other.graph_};
	graph_.swap(graph);
	return *this;
}


template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t 
Network<Vertex, Edge>::GetSourceVertexDescriptor(const edge_t& edge) const
{ return source(edge, graph_); }


template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t 
Network<Vertex, Edge>::GetTargetVertexDescriptor(const edge_t& edge) const
{ return target(edge, graph_); }


template <typename Vertex, typename Edge>
boost::optional<typename Network<Vertex, Edge>::edge_t> 
Network<Vertex, Edge>::GetEdge(
		const vertex_t& source, const vertex_t& target) const { 
	auto boost_edge = boost::edge(source, target, graph_);
	return boost::make_optional(boost_edge.second, boost_edge.first); 
}


template <typename Vertex, typename Edge>
const Edge& Network<Vertex, Edge>::Get(
		const vertex_t& source, const vertex_t& target) const {
	boost::optional<edge_t> edge{GetEdge(source, target)};
	if (!edge) { throw NoEdgeException{}; };
	return operator[](edge.get());
}


template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::Get(
		const vertex_t& source, const vertex_t& target) {
	boost::optional<edge_t> edge{GetEdge(source, target)};
	if (!edge) { throw NoEdgeException{}; };
	return operator[](edge.get());
}


template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::operator()(
		const vertex_t& source, const vertex_t& target) {
	// add the edge if necessary
	if (!GetEdge(source, target))
	{ boost::add_edge(source, target, Edge{}, graph_); }

	// get a reference to the Edge
	boost::optional<edge_t> edge_option{GetEdge(source, target)};
	return operator[](edge_option.get());
}


template <typename Vertex, typename Edge>
void Network<Vertex, Edge>::Save(std::ostream& output_graph_archive) const {
	// create boost archive from ostream and save the graph
	boost::archive::text_oarchive archive{output_graph_archive};
	archive << graph_;
}


template <typename Vertex, typename Edge>
void Network<Vertex, Edge>::Load(std::istream& input_graph_archive) {
	boost::archive::text_iarchive archive{input_graph_archive};
	archive >> graph_;
}


#endif  // NETWORK_H
