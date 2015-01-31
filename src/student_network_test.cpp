#include "student_network.hpp"

#include <algorithm>
#include <string>
#include <sstream>
#include <utility>

#include "gtest/gtest.h"

#include "course.hpp"
#include "graph_builder.hpp"
#include "network.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"


using std::find_if;
using std::string;
using std::stringstream;


static void TestGraphStructure(const StudentNetwork& network);

static StudentNetwork::vertex_t FindStudentId(
		const StudentNetwork& network, StudentId student);


class StudentNetworkTest : public ::testing::Test {
 public:
	StudentNetworkTest() : enrollment_stream{enrollment_tab},
		network{BuildStudentGraphFromTab(enrollment_stream)} {}

 private:
	 stringstream enrollment_stream;

   // must be defined after the private member for initialization reasons
 protected:
	StudentNetwork network;
};


TEST_F(StudentNetworkTest, Construction) { TestGraphStructure(network); }


TEST_F(StudentNetworkTest, Serialization) {
	// save the network
	stringstream archive;
	network.Save(archive);
	TestGraphStructure(network);

	// load the network
	StudentNetwork loaded_network;
	loaded_network.Load(archive);
	TestGraphStructure(loaded_network);
}


void TestGraphStructure(const StudentNetwork& network) {
	StudentNetwork::vertex_t student1{FindStudentId(network, StudentId{312995})};
	StudentNetwork::vertex_t student2{FindStudentId(network, StudentId{500928})};
	StudentNetwork::vertex_t student3{FindStudentId(network, StudentId{147195})};
	StudentNetwork::vertex_t student4{FindStudentId(network, StudentId{352468})};
	StudentNetwork::vertex_t student5{FindStudentId(network, StudentId{567890})};


	EXPECT_EQ(1, network.Get(student1, student2));
	EXPECT_EQ(1, network.Get(student1, student3));
	EXPECT_EQ(1, network.Get(student1, student4));
	EXPECT_THROW(network.Get(student1, student5), NoEdgeException);

	EXPECT_EQ(1, network.Get(student2, student3));
	EXPECT_EQ(1, network.Get(student2, student4));
	EXPECT_THROW(network.Get(student2, student5), NoEdgeException);

	EXPECT_EQ(1, network.Get(student3, student4));
	EXPECT_THROW(network.Get(student3, student5), NoEdgeException);
	EXPECT_EQ(1, network.Get(student4, student5));

	/*
	auto edge1 = network.Get(student1, student2);
	EXPECT_EQ(2u, edge1.size());
	EXPECT_EQ(1u, edge1.count(Course{"ENGLISH", 125}));
	EXPECT_EQ(1u, edge1.count(Course{"CHEM", 210}));

	auto edge2 = network.Get(student1, student3);
	EXPECT_EQ(2u, edge2.size());
	EXPECT_EQ(1u, edge2.count(Course{"ENGLISH", 125}));
	EXPECT_EQ(1u, edge2.count(Course{"AAPTIS", 277}));

	auto edge3 = network.Get(student1, student4);
	EXPECT_EQ(1u, edge3.size());
	EXPECT_EQ(1u, edge3.count(Course{"ENGLISH", 125}));

	EXPECT_THROW(network.Get(student1, student5), NoEdgeException);

	auto edge5 = network.Get(student2, student3);
	EXPECT_EQ(1u, edge5.size());
	EXPECT_EQ(1u, edge5.count(Course{"ENGLISH", 125}));

	auto edge6 = network.Get(student2, student4);
	EXPECT_EQ(1u, edge6.size());
	EXPECT_EQ(1u, edge6.count(Course{"ENGLISH", 125}));

	EXPECT_THROW(network.Get(student2, student5), NoEdgeException);

	auto edge8 = network.Get(student3, student4);
	EXPECT_EQ(2u, edge8.size());
	EXPECT_EQ(1u, edge8.count(Course{"ENGLISH", 125}));
	EXPECT_EQ(1u, edge8.count(Course{"ENVIRON", 311}));

	EXPECT_THROW(network.Get(student3, student5), NoEdgeException);

	auto edge10 = network.Get(student4, student5);
	EXPECT_EQ(1u, edge10.size());
	EXPECT_EQ(1u, edge10.count(Course{"MATH", 425}));
	*/
}


StudentNetwork::vertex_t FindStudentId(
		const StudentNetwork& network, StudentId student) {
	auto it = find_if(network.GetVertices().begin(), network.GetVertices().end(), 
		 [&network, student](const StudentNetwork::vertex_t& vertex_descriptor)
		 { return network[vertex_descriptor] == student; });
	EXPECT_NE(network.GetVertices().end(), network.GetVertices().begin());

	return *it;
}
