#include "student_network.hpp"

#include <algorithm>
#include <string>
#include <sstream>
#include <utility>

#include <boost/graph/adjacency_list.hpp>

#include "gtest/gtest.h"

#include "course.hpp"
#include "graph_builder.hpp"
#include "network.hpp"
#include "student.hpp"


using std::find_if;
using std::string;
using std::stringstream;


static void TestGraphStructure(const StudentNetwork& network);
static StudentNetwork::vertex_t FindStudentId(
		const StudentNetwork& network, StudentId student);


const string sample_tab{
"ID\tSUBJECT\tCATALOGNBR\tCOURSE_CODE\tGRADE\tGPAO\tCUM_GPA\tTOTALCREDITS\t"
"TOTALGRADEPTS\tCOURSECREDIT\tTERM\n"
"312995\tENGLISH\t125\tNA\t3.7\t3.65909090909091\t3.66538461538462\t26\t95.3\t"
"4\t201403\n"
"312995\tCHEM\t210\tCHEM_III\t3.3\t3.73181818181818\t3.66538461538462\t26\t95.3"
"\t4\t201403\n"
"312995\tCHEM\t211\tCHEM_III_LAB\t3\t3.692\t3.66538461538462\t26\t95.3\t1"
"\t201407\n"
"312995\tAAPTIS\t277\tNA\t4\t3.60454545454545\t3.66538461538462\t26\t95.3\t4\t"
"201403\n"
"500928\tENGLISH\t125\tNA\t4\t3.57538461538462\t3.58181818181818\t66\t236.4\t1"
"\t201407\n"
"500928\tCHEM\t210\tCHEM_III\t3.3\t3.73181818181818\t3.66538461538462\t26\t95.3"
"\t4\t201403\n"
"147195\tENGLISH\t125\tNA\t4\t3.57538461538462\t3.58181818181818\t66\t236.4\t1"
"\t201403\n"
"147195\tENVIRON\t311\tNA\t4\t3.55483870967742\t3.58181818181818\t66\t236.4\t4"
"\t201405\n"
"147195\tAAPTIS\t277\tNA\t4\t3.60454545454545\t3.66538461538462\t26\t95.3\t4\t"
"201403\n"
"352468\tENGLISH\t125\tNA\t4\t3.57538461538462\t3.58181818181818\t66\t236.4\t1"
"\t201405\n"
"352468\tENVIRON\t311\tNA\t4\t3.55483870967742\t3.58181818181818\t66\t236.4\t4"
"\t201403\n"
"352468\tMATH\t425\tNA\t4\t4\t4\t27\t108\t3\t201407\n"
"123456\tMATH\t425\tNA\t4\t4\t4\t27\t108\t3\t201407\n"
};


class StudentNetworkTest : public ::testing::Test {
 public:
	StudentNetworkTest() : course_stream_{sample_tab},
						 network{BuildStudentGraphFromTab(course_stream_)} {}


 private:
   stringstream course_stream_;

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
	StudentNetwork::vertex_t student5{FindStudentId(network, StudentId{123456})};

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
}


StudentNetwork::vertex_t FindStudentId(
		const StudentNetwork& network, StudentId student) {
	auto it = find_if(network.GetVertices().begin(), network.GetVertices().end(), 
		 [&network, student](const StudentNetwork::vertex_t& vertex_descriptor)
		 { return network[vertex_descriptor] == student; });
	EXPECT_NE(network.GetVertices().end(), network.GetVertices().begin());

	return *it;
}
