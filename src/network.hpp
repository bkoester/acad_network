#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <iterator>
#include <map>
#include <type_traits>
#include <utility>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/optional.hpp>

#include "adj_mat_serialize.hpp"
#include "bgl_value_iterator.hpp"

class NoEdgeException{};
class NoVertexException{};

// Define abstract templated class for Network.
template <typename Vertex, typename Edge>
class Network {
 public:
	// convenient for dealing with graphs
	using graph_t = boost::adjacency_matrix<boost::undirectedS, Vertex, Edge>;
	using vertex_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
	using edge_t = typename boost::graph_traits<graph_t>::edge_descriptor;
	using degree_t = typename boost::graph_traits<graph_t>::degree_size_type;
	using vertices_size_t = 
		typename boost::graph_traits<graph_t>::vertices_size_type;
	using vertex_iterator_t = 
		typename boost::graph_traits<graph_t>::vertex_iterator;
	using edges_size_t = typename boost::graph_traits<graph_t>::edges_size_type;
	using edge_iterator_t = typename boost::graph_traits<graph_t>::edge_iterator;
	using out_edge_iterator_t = 
		typename boost::graph_traits<graph_t>::out_edge_iterator;

	// Construct empty graph.
	Network();
	// ifstream must contain a boost input archive.
	explicit Network(std::istream& input);
	// Create a network with the given number of vertices.
	explicit Network(long unsigned int num_vertices);
	// Construct a network with a certain starting set of vertices given in the
	// range by the iterators.
	template <typename ForwardIt>
	Network(ForwardIt start, ForwardIt end);
	// Allows a network to be built from a non-boost archive graph format.
	Network(const graph_t& graph);

	vertex_t GetSourceDescriptor(const edge_t& edge) const;
	vertex_t GetTargetDescriptor(const edge_t& edge) const;

	const Vertex& GetSourceValue(const edge_t& edge) const;
	Vertex& GetSourceValue(const edge_t& edge);

	const Vertex& GetTargetValue(const edge_t& edge) const;
	Vertex& GetTargetValue(const edge_t& edge);

	boost::optional<edge_t> GetEdgeDescriptor(
			const vertex_t& source, const vertex_t& target) const;

	// find the descriptor for a vertex given its value, takes O(|V|) time
	vertex_t GetVertexDescriptor(const Vertex& vertex) const;

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
	Edge& operator()(const vertex_t& source, const vertex_t& target, Edge init);
	Edge& operator()(const vertex_t& source, const vertex_t& target);
	Edge& operator()(const Vertex& source, const Vertex& target, Edge init);
	Edge& operator()(const Vertex& source, const Vertex& target);

	// more complex algorithms
	// Returns the number of steps to get from start to any other vertex.
	// If a vertex is disconnected, its distance is returned as 0.
	std::map<Vertex, int> FindUnweightedDistances(vertex_t start) const;
	// Returns the weighted distance to get from start to any other vertex.
	// Uses bundled edge property as weight.
	// If a vertex is disconnected, its distance is returned as 0.
	std::map<Vertex, Edge> FindWeightedDistances(vertex_t start) const;
	// Calculates brandes betweeness centrality of vertices.
	std::map<Vertex, int> CalculateBrandesBetweennessCentrality() const;

	void Save(std::ostream& output_graph_archive) const;
	void Load(std::istream& input_graph_archive);
	void SaveEdgewise(std::ostream& output) const;

	// classes declared to provide iterator access to vertices
	// constructors are private to require access through GetVertices() and
	// GetEdges() functions, which supply the graph parameter to the constructors
	template <typename Adaptor>
	class Properties {
	 public:
		using prop_size_t = decltype(std::declval<Adaptor>().size());

		prop_size_t size() const { return adaptor_.size(); }

	 private:
		friend class Network;
		Properties(const Adaptor& adaptor) : adaptor_{adaptor} {}

		Adaptor adaptor_;
	};

	template <typename Adaptor>
	class Descriptors : public Properties<Adaptor> {
	 public:
		using iterator_t = decltype(std::declval<Adaptor>().Iterate().first);

		iterator_t begin() const { return adaptor_.Iterate().first; }
		iterator_t cbegin() const { return begin(); }

		iterator_t end() const { return adaptor_.Iterate().second; }
		iterator_t cend() const { return end(); }

	 private:
		friend class Network;
		Descriptors(const Adaptor& adaptor) : Properties<Adaptor>{adaptor},
			adaptor_{adaptor} {}

		Adaptor adaptor_;
	};

	template <typename Adaptor, typename Graph>
	class Values : public Properties<Adaptor> {
	 public:
		using parent_t = Properties<Adaptor>;
		using const_value_iterator_t = BglValueIterator<const Graph,
			  decltype(std::declval<Descriptors<Adaptor>>().cbegin())>;
		using value_iterator_t = BglValueIterator<Graph,
			  decltype(std::declval<Descriptors<Adaptor>>().begin())>;

		value_iterator_t begin()
		{ return value_iterator_t{graph_, descriptors_.begin()}; }
		const_value_iterator_t begin() const
		{ return const_value_iterator_t{graph_, descriptors_.begin()}; }
		const_value_iterator_t cbegin() const
		{ return const_value_iterator_t{graph_, descriptors_.cbegin()}; }

		value_iterator_t end()
		{ return value_iterator_t{graph_, descriptors_.end()}; }
		const_value_iterator_t end() const
		{ return const_value_iterator_t{graph_, descriptors_.end()}; }
		const_value_iterator_t cend() const
		{ return const_value_iterator_t{graph_, descriptors_.cend()}; }

	 private:
		friend class Network;
		Values(const Adaptor& adaptor, Graph& graph) : parent_t{adaptor},
			descriptors_{adaptor}, graph_(graph) {}

		Descriptors<Adaptor> descriptors_;
		Graph& graph_;
	};

 private:
	// Forward declarations of adaptor classes defined later.
	class EdgeAdaptor;
	class VertexAdaptor;
	class OutEdgesAdaptor;

 public:
	using vertex_descriptors_t = Descriptors<VertexAdaptor>;
	using const_vertex_values_t = Values<VertexAdaptor, const graph_t>;
	using vertex_values_t = Values<VertexAdaptor, graph_t>;
	using edge_descriptors_t = Descriptors<EdgeAdaptor>;
	using const_edge_values_t = Values<EdgeAdaptor, const graph_t>;
	using edge_values_t = Values<EdgeAdaptor, graph_t>;
	using out_edge_descriptors_t = Descriptors<OutEdgesAdaptor>;
	using const_out_edge_values_t = Values<OutEdgesAdaptor, const graph_t>;
	using out_edge_values_t = Values<OutEdgesAdaptor, graph_t>;

	vertex_descriptors_t GetVertexDescriptors() const 
	{ return vertex_descriptors_t{VertexAdaptor{graph_}}; }
	vertex_values_t GetVertexValues()
	{ return vertex_values_t{VertexAdaptor{graph_}, graph_}; }
	const_vertex_values_t GetVertexValues() const
	{ return const_vertex_values_t{VertexAdaptor{graph_}, graph_}; }

	edge_descriptors_t GetEdgeDescriptors() const 
	{ return edge_descriptors_t{EdgeAdaptor{graph_}}; }
	edge_values_t GetEdgeValues()
	{ return edge_values_t{EdgeAdaptor{graph_}, graph_}; }
	const edge_values_t GetEdgeValues() const
	{ return const_edge_values_t{EdgeAdaptor{graph_}, graph_}; }

	out_edge_descriptors_t GetOutEdgeDescriptors(vertex_t vertex) const
	{ return out_edge_descriptors_t{OutEdgesAdaptor{vertex, graph_}}; }
	out_edge_values_t GetOutEdgeValues(vertex_t vertex)
	{ return out_edge_values_t{OutEdgesAdaptor{vertex, graph_}, graph_}; }
	const_out_edge_values_t GetOutEdgeValues(vertex_t vertex) const
	{ return const_out_edge_values_t{OutEdgesAdaptor{vertex, graph_}, graph_}; }

 private:
	class VertexAdaptor {
	 public:
		VertexAdaptor(const graph_t& graph) : graph_(graph) {}
		VertexAdaptor(const VertexAdaptor& other) : graph_(other.graph_) {}

		std::pair<vertex_iterator_t, vertex_iterator_t> Iterate() const
		{ return boost::vertices(graph_); }
		vertices_size_t size() const { return boost::num_vertices(graph_); }

	 private:
		const graph_t& graph_;
	};

	class EdgeAdaptor {
	 public:
		EdgeAdaptor(const graph_t& graph) : graph_(graph) {}
		EdgeAdaptor(const EdgeAdaptor& other) : graph_(other.graph_) {}

		std::pair<edge_iterator_t, edge_iterator_t> Iterate() const 
		{ return boost::edges(graph_); }
		edges_size_t size() const { return boost::num_edges(graph_); }

	 private:
		const graph_t& graph_;
	};

	class OutEdgesAdaptor {
	 public:
		OutEdgesAdaptor(vertex_t vertex, const graph_t& graph) :
			vertex_{vertex}, graph_(graph) {}

		std::pair<out_edge_iterator_t, out_edge_iterator_t> Iterate() const
		{ return boost::out_edges(vertex_, graph_); }
		degree_t size() const { return boost::out_degree(vertex_, graph_); }

	 private:
		vertex_t vertex_;
		const graph_t& graph_;
	};

	graph_t graph_;
};


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network() : graph_{0} {}


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(long unsigned int num_vertices) :
	graph_{num_vertices} {}


template <typename Vertex, typename Edge>
template <typename ForwardIt>
Network<Vertex, Edge>::Network(ForwardIt first, ForwardIt last) : 
		graph_{static_cast<long unsigned int>(std::distance(first, last))} {
	auto it = first;
	for (auto& vertex : GetVertexValues()) { vertex = *it++; }
}


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(std::istream& input) : graph_{0} { Load(input); }


template <typename Vertex, typename Edge>
Network<Vertex, Edge>::Network(const graph_t& graph) : graph_{graph} {}


template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t 
Network<Vertex, Edge>::GetSourceDescriptor(const edge_t& edge) const
{ return source(edge, graph_); }


template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t 
Network<Vertex, Edge>::GetTargetDescriptor(const edge_t& edge) const
{ return target(edge, graph_); }


template <typename Vertex, typename Edge>
const Vertex& Network<Vertex, Edge>::GetSourceValue(const edge_t& edge) const
{ return operator[](GetSourceDescriptor(edge)); }


template <typename Vertex, typename Edge>
Vertex& Network<Vertex, Edge>::GetSourceValue(const edge_t& edge)
{ return operator[](GetSourceDescriptor(edge)); }


template <typename Vertex, typename Edge>
const Vertex& Network<Vertex, Edge>::GetTargetValue(const edge_t& edge) const
{ return operator[](GetTargetDescriptor(edge)); }


template <typename Vertex, typename Edge>
Vertex& Network<Vertex, Edge>::GetTargetValue(const edge_t& edge)
{ return operator[](GetTargetDescriptor(edge)); }


template <typename Vertex, typename Edge>
boost::optional<typename Network<Vertex, Edge>::edge_t> 
Network<Vertex, Edge>::GetEdgeDescriptor(
		const vertex_t& source, const vertex_t& target) const { 
	auto boost_edge = boost::edge(source, target, graph_);
	return boost::make_optional(boost_edge.second, boost_edge.first); 
}


template <typename Vertex, typename Edge>
const Edge& Network<Vertex, Edge>::Get(
		const vertex_t& source, const vertex_t& target) const {
	boost::optional<edge_t> edge{GetEdgeDescriptor(source, target)};
	if (!edge) { throw NoEdgeException{}; };
	return operator[](edge.get());
}


template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::Get(
		const vertex_t& source, const vertex_t& target) {
	boost::optional<edge_t> edge{GetEdgeDescriptor(source, target)};
	if (!edge) { throw NoEdgeException{}; };
	return operator[](edge.get());
}


template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::operator()(
		const vertex_t& source, const vertex_t& target, Edge init) {
	// add the edge if necessary
	if (!GetEdgeDescriptor(source, target))
	{ boost::add_edge(source, target, init, graph_); }

	// get a reference to the Edge
	boost::optional<edge_t> edge_option{GetEdgeDescriptor(source, target)};
	return operator[](edge_option.get());
}


template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::operator()(
		const vertex_t& source, const vertex_t& target) 
{ return operator()(source, target, Edge{}); }

template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::operator()(
		const Vertex& source, const Vertex& target, Edge init) {
	auto source_vertex = GetVertexDescriptor(source);
	auto target_vertex = GetVertexDescriptor(target);
	return operator()(source_vertex, target_vertex, init);
}


template <typename Vertex, typename Edge>
Edge& Network<Vertex, Edge>::operator()(
		const Vertex& source, const Vertex& target) 
{ return operator()(source, target, Edge{}); }


template <typename Vertex, typename Edge>
typename Network<Vertex, Edge>::vertex_t
Network<Vertex, Edge>::GetVertexDescriptor(const Vertex& vertex) const {
	auto vertex_it = std::find_if(std::begin(GetVertexDescriptors()),
			std::end(GetVertexDescriptors()), [&vertex, this](vertex_t vertex_d)
			{ return operator[](vertex_d) == vertex; });

	if (vertex_it == std::end(GetVertexDescriptors()))
	{ throw NoVertexException{}; }
	return *vertex_it;
}


template <typename Vertex, typename Edge>
std::map<Vertex, int> Network<Vertex, Edge>::FindUnweightedDistances(
		vertex_t start) const {
	std::vector<vertices_size_t> distances(GetVertexValues().size(), 0);
	// perform a BFS on the vertex
	boost::breadth_first_search(graph_, start, boost::visitor(
				boost::make_bfs_visitor(boost::record_distances(
						boost::make_iterator_property_map(
							std::begin(distances), 
							get(boost::vertex_index, graph_)),
						boost::on_tree_edge()))));

	// return with the actual bundled vertex property, the descriptor is useless
	std::map<Vertex, int> value_distances;
	// distances indices are vertex descriptors, so no std algorithm here
	for (auto vertex_descriptor = 0u; vertex_descriptor < distances.size(); 
			++vertex_descriptor) {
		value_distances[operator[](vertex_descriptor)] = 
			distances[vertex_descriptor];
	}

	return value_distances;
}

template <typename Vertex, typename Edge>
std::map<Vertex, Edge> Network<Vertex, Edge>::FindWeightedDistances(
		vertex_t start) const {
	std::vector<Edge> distances(GetVertexValues().size(), 0);
	// perform a BFS on the vertex
	boost::dijkstra_shortest_paths(graph_, start, 
			boost::weight_map(boost::get(boost::edge_bundle, graph_)).
			distance_map(boost::make_iterator_property_map(
					std::begin(distances),
					get(boost::vertex_index, graph_))));

	std::map<Vertex, Edge> value_distances;
	// distances indices are vertex descriptors, so no std algorithm here
	for (auto vertex_descriptor = 0u; vertex_descriptor < distances.size();
			++vertex_descriptor) {
		auto distance = distances[vertex_descriptor];
		// test if distance is maximum (change to zero)
		if (std::numeric_limits<Edge>::max() - distance == 0) { distance = 0; }
		value_distances[operator[](vertex_descriptor)] = distance;
	}

	return value_distances;

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

template <typename Vertex, typename Edge>
void Network<Vertex, Edge>::SaveEdgewise(std::ostream& output) const {
	// output all edges in "vertex1 vertex2 edge" form
	for (const auto& edge_d : GetEdgeDescriptors()) {
		output << GetSourceValue(edge_d) << "\t" << GetTargetValue(edge_d)
			   << "\t" << operator[](edge_d) << std::endl;
	}
}


#endif  // NETWORK_H
