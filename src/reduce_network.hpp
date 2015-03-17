#ifndef REDUCE_NETWORK_H
#define REDUCE_NETWORK_H

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <set>
#include <utility>

#include "network.hpp"


// reduce the network using functions that map every vertex in the original onto
// a new space and every edge in the original onto a new space
template <typename InputVertex, typename InputEdge, typename VertexFunc, 
		  typename EdgeFunc, typename Init>
Network<decltype(std::declval<VertexFunc>()(InputVertex{})), 
		decltype(std::declval<EdgeFunc>()(InputEdge{}, Init{}))>
ReduceNetwork(const Network<InputVertex, InputEdge>& input_network, 
			   VertexFunc vertex_func, EdgeFunc edge_func, Init init) {
	// we'll have to do an initial loop to get all the vertices because BGL's
	// add_vertex does not exist for adjacency_matrix yet
	using reduced_vertex_t = decltype(std::declval<VertexFunc>()(InputVertex{}));
	using reduced_edge_t = decltype(std::declval<EdgeFunc>()(
			InputEdge{}, Init{}));
	using reduced_network_t = Network<reduced_vertex_t, reduced_edge_t>;
	std::set<reduced_vertex_t> reduced_vertices;
	std::transform(std::begin(input_network.GetVertexValues()), 
		 std::end(input_network.GetVertexValues()),
		 std::inserter(reduced_vertices, end(reduced_vertices)),
		 [vertex_func](const InputVertex& vertex) 
			{ return vertex_func(vertex); });

	// create the new network with the given set of starting vertices
	Network<reduced_vertex_t, reduced_edge_t> reduced{
		std::begin(reduced_vertices), std::end(reduced_vertices)};

	// make a map of Vertex => descriptor
	std::map<reduced_vertex_t, typename reduced_network_t::vertex_t> 
		reduced_vertex_map;
	std::transform(std::begin(reduced.GetVertexDescriptors()), 
				   std::end(reduced.GetVertexDescriptors()),
				   std::inserter(reduced_vertex_map, end(reduced_vertex_map)),
				   [&reduced](typename reduced_network_t::vertex_t vertex_d)
				   { return std::make_pair(reduced[vertex_d], vertex_d); });

	// transform the full network into the reduced network by edge
	for (auto edge_d: input_network.GetEdgeDescriptors()) {
		auto reduced_source = reduced_vertex_map[vertex_func(
				input_network.GetSourceValue(edge_d))];
		auto reduced_target = reduced_vertex_map[vertex_func(
				input_network.GetTargetValue(edge_d))];
		auto old_edge_value = reduced(reduced_source, reduced_target, init);
		auto updated_edge_value = edge_func(input_network[edge_d], 
											old_edge_value);
		reduced(reduced_source, reduced_target) = updated_edge_value;
	}

	return reduced;
}


#endif  // REDUCE_NETWORK_H
