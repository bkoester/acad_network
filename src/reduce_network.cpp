#include "reduce_network.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <unordered_set>


using std::begin; using std::end; using std::inserter;
using std::string;
using std::transform;
using std::unordered_set;


template <typename InputVertex, typename InputEdge, typename VertexFunc, 
		  typename EdgeFunc, typename Init>
Network<decltype(std::declval<VertexFunc>()(InputVertex{})), 
		decltype(std::declval<EdgeFunc>()(InputEdge{}, Init{}))>

