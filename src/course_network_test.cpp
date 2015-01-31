#include "course_network.hpp"

#include <string>
#include <sstream>
#include <utility>

#include "gtest/gtest.h"

#include "graph_builder.hpp"
#include "test_data_streams.hpp"


using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::stringstream;


static void TestGraphStructure(const CourseNetwork& network);


class CourseNetworkTest : public ::testing::Test {
 public:
	CourseNetworkTest() : enrollment_stream{enrollment_tab},
		network{BuildCourseGraphFromTab(enrollment_stream)} {}

 private:
	stringstream enrollment_stream;

	// must be defined after the private member for initialization reasons
 protected:
	CourseNetwork network;
};


TEST_F(CourseNetworkTest, Construction) { TestGraphStructure(network); }


TEST_F(CourseNetworkTest, Serialization) {
	// save the network
	stringstream archive;
	network.Save(archive);
	TestGraphStructure(network);

	// load the network
	CourseNetwork loaded_network;
	loaded_network.Load(archive);
	TestGraphStructure(loaded_network);
}


void TestGraphStructure(const CourseNetwork& network) {
	CourseNetwork::vertex_t english125_vertex{network.GetVertex(
			Course{"ENGLISH", 125})};
	CourseNetwork::vertex_t chem210_vertex{network.GetVertex(
			Course{"CHEM", 210})};
	CourseNetwork::vertex_t chem211_vertex{network.GetVertex(
			Course{"CHEM", 211})};
	CourseNetwork::vertex_t aaptis277_vertex{network.GetVertex(
			Course{"AAPTIS", 277})};
	CourseNetwork::vertex_t environ311_vertex{network.GetVertex(
			Course{"ENVIRON", 311})};
	CourseNetwork::vertex_t math425_vertex{network.GetVertex(
			Course{"MATH", 425})};

	// test edges that exist
	EXPECT_EQ(2, network.CalculateValue(english125_vertex, chem210_vertex));

	// test both ways for this one
	EXPECT_EQ(2, network.CalculateValue(chem210_vertex, english125_vertex));

	EXPECT_EQ(1, network.CalculateValue(english125_vertex, chem211_vertex));
	EXPECT_EQ(2, network.CalculateValue(english125_vertex, aaptis277_vertex));
	EXPECT_EQ(2, network.CalculateValue(english125_vertex, environ311_vertex));

	EXPECT_EQ(1, network.CalculateValue(chem210_vertex, chem211_vertex));
	EXPECT_EQ(1, network.CalculateValue(chem210_vertex, aaptis277_vertex));
	EXPECT_EQ(0, network.CalculateValue(chem210_vertex, environ311_vertex));
	// test non-existent edge both ways
	EXPECT_EQ(0, network.CalculateValue(environ311_vertex, chem210_vertex));

	EXPECT_EQ(1, network.CalculateValue(chem211_vertex, aaptis277_vertex));
	EXPECT_EQ(0, network.CalculateValue(chem211_vertex, environ311_vertex));
	EXPECT_EQ(0, network.CalculateValue(chem211_vertex, math425_vertex));
	
	EXPECT_EQ(1, network.CalculateValue(aaptis277_vertex, environ311_vertex));
	EXPECT_EQ(0, network.CalculateValue(aaptis277_vertex, math425_vertex));

	EXPECT_EQ(1, network.CalculateValue(environ311_vertex, math425_vertex));
}
