#include "network_structure_test.hpp"

#include <algorithm>

#include "gtest/gtest.h"

#include "course.hpp"
#include "course_network.hpp"
#include "student.hpp"
#include "student_network.hpp"


using std::find_if;


static StudentNetwork::vertex_t FindStudentId(
		const StudentNetwork& network, Student::Id student);


void TestCourseNetworkStructure(const CourseNetwork& network) {
	CourseNetwork::vertex_t english125_vertex{network.GetVertex(
			Course{"ENGLISH", 125, 0})};
	CourseNetwork::vertex_t chem210_vertex{network.GetVertex(
			Course{"CHEM", 210, 0})};
	CourseNetwork::vertex_t chem211_vertex{network.GetVertex(
			Course{"CHEM", 211, 0})};
	CourseNetwork::vertex_t aaptis277_vertex{network.GetVertex(
			Course{"AAPTIS", 277, 0})};
	CourseNetwork::vertex_t environ311_vertex{network.GetVertex(
			Course{"ENVIRON", 311, 0})};
	CourseNetwork::vertex_t math425_vertex{network.GetVertex(
			Course{"MATH", 425, 0})};

	// test edge to itself
	EXPECT_EQ(0, network.CalculateValue(english125_vertex, english125_vertex));
	// test edges that exist
	EXPECT_EQ(2, network.CalculateValue(english125_vertex, chem210_vertex));

	// test both ways for this one
	EXPECT_EQ(2, network.CalculateValue(chem210_vertex, english125_vertex));

	EXPECT_EQ(1, network.CalculateValue(english125_vertex, chem211_vertex));
	EXPECT_EQ(2, network.CalculateValue(english125_vertex, aaptis277_vertex));
	EXPECT_EQ(2, network.CalculateValue(english125_vertex, environ311_vertex));

	EXPECT_EQ(0, network.CalculateValue(chem210_vertex, chem210_vertex));
	EXPECT_EQ(1, network.CalculateValue(chem210_vertex, chem211_vertex));
	EXPECT_EQ(1, network.CalculateValue(chem210_vertex, aaptis277_vertex));
	EXPECT_EQ(0, network.CalculateValue(chem210_vertex, environ311_vertex));
	// test non-existent edge both ways
	EXPECT_EQ(0, network.CalculateValue(environ311_vertex, chem210_vertex));

	EXPECT_EQ(0, network.CalculateValue(chem211_vertex, chem211_vertex));
	EXPECT_EQ(1, network.CalculateValue(chem211_vertex, aaptis277_vertex));
	EXPECT_EQ(0, network.CalculateValue(chem211_vertex, environ311_vertex));
	EXPECT_EQ(0, network.CalculateValue(chem211_vertex, math425_vertex));
	
	EXPECT_EQ(0, network.CalculateValue(aaptis277_vertex, aaptis277_vertex));
	EXPECT_EQ(1, network.CalculateValue(aaptis277_vertex, environ311_vertex));
	EXPECT_EQ(0, network.CalculateValue(aaptis277_vertex, math425_vertex));

	EXPECT_EQ(0, network.CalculateValue(environ311_vertex, environ311_vertex));
	EXPECT_EQ(1, network.CalculateValue(environ311_vertex, math425_vertex));
}

void TestStudentNetworkStructure(const StudentNetwork& network) {
	StudentNetwork::vertex_t student1{FindStudentId(network, Student::Id{312995})};
	StudentNetwork::vertex_t student2{FindStudentId(network, Student::Id{500928})};
	StudentNetwork::vertex_t student3{FindStudentId(network, Student::Id{147195})};
	StudentNetwork::vertex_t student4{FindStudentId(network, Student::Id{352468})};
	StudentNetwork::vertex_t student5{FindStudentId(network, Student::Id{567890})};


	EXPECT_THROW(network.Get(student1, student1), NoEdgeException);
	EXPECT_EQ(3.0, network.Get(student1, student2));
	EXPECT_EQ(3.0, network.Get(student1, student3));
	EXPECT_EQ(1.0, network.Get(student1, student4));
	EXPECT_THROW(network.Get(student1, student5), NoEdgeException);

	EXPECT_THROW(network.Get(student2, student2), NoEdgeException);
	EXPECT_EQ(1.0, network.Get(student2, student3));
	EXPECT_EQ(1.0, network.Get(student2, student4));
	EXPECT_THROW(network.Get(student2, student5), NoEdgeException);

	EXPECT_THROW(network.Get(student3, student3), NoEdgeException);
	EXPECT_EQ(3.0, network.Get(student3, student4));
	EXPECT_THROW(network.Get(student3, student5), NoEdgeException);

	EXPECT_THROW(network.Get(student4, student4), NoEdgeException);
	EXPECT_EQ(1.5, network.Get(student4, student5));

	EXPECT_THROW(network.Get(student5, student5), NoEdgeException);

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
		const StudentNetwork& network, Student::Id student) {
	auto it = find_if(network.GetVertexDescriptors().begin(),
			network.GetVertexDescriptors().end(),
			[&network, student](StudentNetwork::vertex_t vertex_descriptor)
			{ return network[vertex_descriptor] == student; });
	EXPECT_NE(network.GetVertexValues().end(), 
			  network.GetVertexValues().begin());

	return *it;
}
