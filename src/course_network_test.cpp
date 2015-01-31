#include "course_network.hpp"

#include <sstream>

#include <boost/graph/adjacency_list.hpp>
#include "gtest/gtest.h"

#include "graph_builder.hpp"
#include "network_structure_test.hpp"
#include "test_data_streams.hpp"


using std::stringstream;

using boost::add_edge;
using boost::add_vertex;


class CourseNetworkTest : public ::testing::Test {
 public:
	 void SetUp() override {
		 auto english125_vertex = add_vertex(Course{"ENGLISH", 125}, graph_);
		 auto chem210_vertex = add_vertex(Course{"CHEM", 210}, graph_);
		 auto chem211_vertex = add_vertex(Course{"CHEM", 211}, graph_);
		 auto aaptis277_vertex = add_vertex(Course{"AAPTIS", 277}, graph_);
		 auto environ311_vertex = add_vertex(Course{"ENVIRON", 311}, graph_);
		 auto math425_vertex = add_vertex(Course{"MATH", 425}, graph_);

		 add_edge(english125_vertex, chem210_vertex, 2, graph_);
		 add_edge(english125_vertex, chem211_vertex, 1, graph_);
		 add_edge(english125_vertex, aaptis277_vertex, 2, graph_);
		 add_edge(english125_vertex, environ311_vertex, 2, graph_);
		 add_edge(chem210_vertex, chem211_vertex, 1, graph_);
		 add_edge(chem210_vertex, aaptis277_vertex, 1, graph_);
		 add_edge(chem211_vertex, aaptis277_vertex, 1, graph_);
		 add_edge(aaptis277_vertex, environ311_vertex, 1, graph_);
		 add_edge(environ311_vertex, math425_vertex, 1, graph_);

		 network = CourseNetwork{graph_};
	 }

 private:
	CourseNetwork::graph_t graph_;

	// must be defined after the private member for initialization reasons
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
