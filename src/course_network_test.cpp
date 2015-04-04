#include "course_network.hpp"

#include <sstream>

#include <boost/graph/adjacency_matrix.hpp>
#include "gtest/gtest.h"

#include "graph_builder.hpp"
#include "network_structure_test.hpp"
#include "test_data_streams.hpp"


using std::stringstream;

using boost::add_edge;
using boost::vertex;


class CourseNetworkTest : public ::testing::Test {
 public:
	 void SetUp() override {
		 CourseNetwork::graph_t graph{6};

		 // assign the vertices
		 auto english125_vertex = vertex(0, graph);
		 graph[english125_vertex] = Course::Id{"ENGLISH", 125, 0};
		 auto chem210_vertex = vertex(1, graph);
		 graph[chem210_vertex] = Course::Id{"CHEM", 210, 0};
		 auto chem211_vertex = vertex(2, graph);
		 graph[chem211_vertex] = Course::Id{"CHEM", 211, 0};
		 auto aaptis277_vertex = vertex(3, graph);
		 graph[aaptis277_vertex] = Course::Id{"AAPTIS", 277, 0};
		 auto environ311_vertex = vertex(4, graph);
		 graph[environ311_vertex] = Course::Id{"ENVIRON", 311, 0};
		 auto math425_vertex = vertex(5, graph);
		 graph[math425_vertex] = Course::Id{"MATH", 425, 0};

		 // assign the edges
		 add_edge(english125_vertex, chem210_vertex, 2, graph);
		 add_edge(english125_vertex, chem211_vertex, 1, graph);
		 add_edge(english125_vertex, aaptis277_vertex, 2, graph);
		 add_edge(english125_vertex, environ311_vertex, 2, graph);
		 add_edge(chem210_vertex, chem211_vertex, 1, graph);
		 add_edge(chem210_vertex, aaptis277_vertex, 1, graph);
		 add_edge(chem211_vertex, aaptis277_vertex, 1, graph);
		 add_edge(aaptis277_vertex, environ311_vertex, 1, graph);
		 add_edge(environ311_vertex, math425_vertex, 1, graph);

		 network = CourseNetwork{graph};
	 }

 protected:
	CourseNetwork network;
};


TEST_F(CourseNetworkTest, Serialization) {
	// save the network
	stringstream archive;
	network.Save(archive);
	TestCourseNetworkStructure(network);

	// load the network
	CourseNetwork loaded_network;
	loaded_network.Load(archive);
	TestCourseNetworkStructure(loaded_network);
}
