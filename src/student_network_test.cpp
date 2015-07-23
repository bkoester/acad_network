#include "student_network.hpp"

#include <algorithm>
#include <sstream>

#include <boost/graph/adjacency_matrix.hpp>
#include "gtest/gtest.h"

#include "course.hpp"
#include "graph_builder.hpp"
#include "network.hpp"
#include "network_structure_test.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"


using std::find_if;
using std::stringstream;

using boost::add_edge;
using boost::vertex;


class StudentNetworkTest : public ::testing::Test {
 public:
	void SetUp() override {
		StudentNetwork::graph_t graph{5};

		// add vertices
		auto student1 = vertex(0, graph);
		graph[student1] = Student::Id{147195};
		auto student2 = vertex(1, graph);
		graph[student2] = Student::Id{312995};
		auto student3 = vertex(2, graph);
		graph[student3] = Student::Id{352468};
		auto student4 = vertex(3, graph);
		graph[student4] = Student::Id{500928};
		auto student5 = vertex(4, graph);
		graph[student5] = Student::Id{567890};

		// add edges
		add_edge(student1, student2, 3.0, graph);
		add_edge(student1, student3, 3.0, graph);
		add_edge(student1, student4, 1.0, graph);

		add_edge(student2, student3, 1.0, graph);
		add_edge(student2, student4, 3.0, graph);

		add_edge(student3, student4, 1.0, graph);
		add_edge(student3, student5, 1.5, graph);

		network = StudentNetwork{graph};

		// try with a disconnected graph
		StudentNetwork::graph_t disconnected_graph{2};

		// add vertices
		auto disconnected_student1 = vertex(0, disconnected_graph);
		disconnected_graph[disconnected_student1] = Student::Id{1};
		auto disconnected_student2 = vertex(1, disconnected_graph);
		disconnected_graph[disconnected_student2] = Student::Id{2};

		disconnected_network = StudentNetwork{disconnected_graph};
	}

 protected:
	StudentNetwork network, disconnected_network;
};


TEST_F(StudentNetworkTest, Serialization) {
	// save the network
	stringstream archive;
	network.Save(archive);
	TestStudentNetworkStructure(network);

	// load the network
	StudentNetwork loaded_network;
	loaded_network.Load(archive);
	TestStudentNetworkStructure(loaded_network);
}


TEST_F(StudentNetworkTest, GetVertexDescriptor) {
	Student::Id student_id1{312995};
	Student::Id student_id2{500928};
	
	Student::Id found_id1{network[network.GetVertexDescriptor(student_id1)]};
	Student::Id found_id2{network[network.GetVertexDescriptor(student_id2)]};

	EXPECT_EQ(student_id1, found_id1);
	EXPECT_EQ(student_id2, found_id2);

	EXPECT_THROW(network.GetVertexDescriptor(Student::Id(0)), NoVertexException);
}


TEST_F(StudentNetworkTest, FindUnweightedDistance) {
	auto distances = network.FindUnweightedDistances(
			network.GetVertexDescriptor(Student::Id(312995)));

	EXPECT_EQ(5u, distances.size());
	EXPECT_EQ(0, distances[Student::Id(312995)]);
	EXPECT_EQ(1, distances[Student::Id(500928)]);
	EXPECT_EQ(1, distances[Student::Id(147195)]);
	EXPECT_EQ(1, distances[Student::Id(352468)]);
	EXPECT_EQ(2, distances[Student::Id(567890)]);

	auto disconnected_distances = disconnected_network.FindUnweightedDistances(
			disconnected_network.GetVertexDescriptor(Student::Id(1)));

	EXPECT_EQ(2u, disconnected_distances.size());
	EXPECT_EQ(0, disconnected_distances[Student::Id(1)]);
	EXPECT_EQ(0, disconnected_distances[Student::Id(2)]);
}


TEST_F(StudentNetworkTest, FindWeightedDistance) {
	auto distances = network.FindWeightedDistances(network.GetVertexDescriptor(
				Student::Id(312995)));

	EXPECT_EQ(5u, distances.size());
	EXPECT_DOUBLE_EQ(0., distances[Student::Id(312995)]);
	EXPECT_DOUBLE_EQ(2., distances[Student::Id(500928)]);
	EXPECT_DOUBLE_EQ(3., distances[Student::Id(147195)]);
	EXPECT_DOUBLE_EQ(1., distances[Student::Id(352468)]);
	EXPECT_DOUBLE_EQ(2.5, distances[Student::Id(567890)]);

	auto disconnected_distances = disconnected_network.FindWeightedDistances(
			disconnected_network.GetVertexDescriptor(Student::Id(1)));

	EXPECT_EQ(2u, disconnected_distances.size());
	EXPECT_DOUBLE_EQ(0., disconnected_distances[Student::Id(1)]);
	EXPECT_DOUBLE_EQ(0., disconnected_distances[Student::Id(2)]);
}

TEST_F(StudentNetworkTest, CalculateUnweightedBetweennessCentrality) {
	auto centralities = network.CalculateUnweightedBetweennessCentrality();

	EXPECT_EQ(5u, centralities.size());
	EXPECT_DOUBLE_EQ(0., centralities[Student::Id(147195)]);
	EXPECT_DOUBLE_EQ(0., centralities[Student::Id(312995)]);
	EXPECT_DOUBLE_EQ(3., centralities[Student::Id(352468)]);
	EXPECT_DOUBLE_EQ(0., centralities[Student::Id(500928)]);
	EXPECT_DOUBLE_EQ(0., centralities[Student::Id(567890)]);

	auto disconnected_centralities =
		disconnected_network.CalculateUnweightedBetweennessCentrality();

	EXPECT_EQ(2u, disconnected_centralities.size());
	EXPECT_DOUBLE_EQ(0., disconnected_centralities[Student::Id(1)]);
	EXPECT_DOUBLE_EQ(0., disconnected_centralities[Student::Id(2)]);

	// Build network shaped like a diamond split into two triangles.
	StudentNetwork::graph_t diamond_graph{4};

	auto student1 = vertex(0, diamond_graph);
	diamond_graph[student1] = Student::Id{1};
	auto student2 = vertex(1, diamond_graph);
	diamond_graph[student2] = Student::Id{2};
	auto student3 = vertex(2, diamond_graph);
	diamond_graph[student3] = Student::Id{3};
	auto student4 = vertex(3, diamond_graph);
	diamond_graph[student4] = Student::Id{4};
	
	// add edges
	add_edge(student1, student2, 1.0, diamond_graph);
	add_edge(student2, student3, 1.0, diamond_graph);
	add_edge(student2, student4, 1.0, diamond_graph);
	add_edge(student3, student4, 1.0, diamond_graph);
	add_edge(student4, student1, 1.0, diamond_graph);

	auto diamond_network = StudentNetwork{diamond_graph};

	auto centralities2 = 
		diamond_network.CalculateUnweightedBetweennessCentrality();

	EXPECT_EQ(4u, centralities2.size());
	EXPECT_DOUBLE_EQ(0., centralities2[Student::Id(1)]);
	EXPECT_DOUBLE_EQ(0.5, centralities2[Student::Id(2)]);
	EXPECT_DOUBLE_EQ(0., centralities2[Student::Id(3)]);
	EXPECT_DOUBLE_EQ(0.5, centralities2[Student::Id(4)]);
}
