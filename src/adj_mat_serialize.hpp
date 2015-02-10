#ifndef ADJ_MAT_SERIALIZE_HPP
#define ADJ_MAT_SERIALIZE_HPP

#include <map>
#include <utility>

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/pending/property_serialize.hpp>
#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/split_free.hpp>

/* This file heavily inspired by the included boost/graph/adj_list_serialize.hpp
 * file with some changes for my own style and to make loading work with an
 * adjacency matrix, which is not default constructible. */

namespace boost { 

namespace serialization {

template<typename Archive, typename D, typename VP, typename EP, typename GP, 
	typename EL>
inline void save(Archive & ar,
		const boost::adjacency_matrix<D, VP, EP, GP, EL>& graph,
		const unsigned int /* file_version */) {
	typedef boost::adjacency_matrix<D, VP, EP, GP, EL> Graph;
	typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

	int V = num_vertices(graph);
	int E = num_edges(graph);
	ar << BOOST_SERIALIZATION_NVP(V);
	ar << BOOST_SERIALIZATION_NVP(E);

	// assign indices to vertices
	std::map<Vertex, long> indices;
	long num{0};
	BGL_FORALL_VERTICES_T(v, graph, Graph) {
		indices[v] = num++;
		ar << serialization::make_nvp(
				"vertex_property", get(vertex_all_t{}, graph, v));
	}

	// write edges
	BGL_FORALL_EDGES_T(e, graph, Graph) {
		ar << serialization::make_nvp("u", indices[source(e,graph)]);
		ar << serialization::make_nvp("v", indices[target(e,graph)]);
		ar << serialization::make_nvp(
				"edge_property", get(edge_all_t{}, graph, e));
	}
}


template <typename Archive, typename D, typename VP, typename EP, typename GP, 
		 typename EL>
inline void load(Archive & ar,
				 boost::adjacency_matrix<D, VP, EP, GP, EL>& graph,
				 const unsigned int /* file_version */) {
	typedef boost::adjacency_matrix<D, VP, EP, GP, EL> Graph;
	typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename graph_traits<Graph>::edge_descriptor Edge;

	long unsigned V;
	ar >> BOOST_SERIALIZATION_NVP(V);
	long unsigned E;
	ar >> BOOST_SERIALIZATION_NVP(E);
	// create temporary graph object to hold values because we can't add
	// vertices to a constructed adjacency matrix
	Graph tmp{V};

	// read vertices
	std::vector<Vertex> verts(V);
	long index{0};
	while (V-- > 0) {
		Vertex v{vertex(index, tmp)};
		verts[index++] = v;
		ar >> serialization::make_nvp(
				"vertex_property", get(vertex_all_t{}, tmp, v));
	}

	// read edges
	while (E-- > 0) {
		long u{0}; long v{0};
		ar >> BOOST_SERIALIZATION_NVP(u);
		ar >> BOOST_SERIALIZATION_NVP(v);
		Edge e{add_edge(verts[u], verts[v], EP{}, tmp).first};
		ar >> serialization::make_nvp(
				"edge_property", get(edge_all_t{}, tmp, e));
	}

	// assign the temporary graph object to the input graph object
	std::swap(graph, tmp);
}

template <typename Archive, typename D, typename VP, typename EP, typename GP, 
		 typename EL>
inline void serialize(Archive & ar,
		boost::adjacency_matrix<D, VP, EP, GP, EL>& graph,
		const unsigned int file_version){
    boost::serialization::split_free(ar, graph, file_version);
}

} // serialization
} // boost


#endif  // ADJ_MAT_SERIALIZE_HPP
