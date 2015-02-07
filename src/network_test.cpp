#include "network.hpp"

#include "gtest/gtest.h"


class NetworkTest : public ::testing::Test {
 public:
	void SetUp() override {
		network = Network<int, int>{6};

		int vertex_value{0};
		auto vertex_value_it = network.GetVertexValues().begin();
		*vertex_value_it = vertex_value++;
		++vertex_value_it;
		*vertex_value_it = vertex_value++;
		++vertex_value_it;
		*vertex_value_it = vertex_value++;
		++vertex_value_it;
		*vertex_value_it = vertex_value++;
		++vertex_value_it;
		*vertex_value_it = vertex_value++;
		++vertex_value_it;
		*vertex_value_it = vertex_value++;
		

		auto vertex_descriptor_it1 = network.GetVertexDescriptors().begin();
		auto vertex_descriptor_it2 = ++network.GetVertexDescriptors().begin();
		auto edge_value = 100;

		network(*vertex_descriptor_it1++, *vertex_descriptor_it2++) = 
			edge_value++;
		network(*vertex_descriptor_it1++, *vertex_descriptor_it2++) = 
			edge_value++;
		network(*vertex_descriptor_it1++, *vertex_descriptor_it2++) = 
			edge_value++;
		network(*vertex_descriptor_it1++, *vertex_descriptor_it2++) = 
			edge_value++;
		network(*vertex_descriptor_it1++, *vertex_descriptor_it2++) = 
			edge_value++;
	}

 protected:
	Network<int, int> network;
};


TEST_F(NetworkTest, Structure) {
	auto vertex_descriptor_it1 = network.GetVertexDescriptors().begin();
	auto vertex_descriptor_it2 = ++network.GetVertexDescriptors().begin();

	EXPECT_EQ(0, network[*vertex_descriptor_it1]);
	EXPECT_EQ(100, network.Get(
				*vertex_descriptor_it1++, *vertex_descriptor_it2++));

	EXPECT_EQ(1, network[*vertex_descriptor_it1]);
	EXPECT_EQ(101, network.Get(
				*vertex_descriptor_it1++, *vertex_descriptor_it2++));

	EXPECT_EQ(2, network[*vertex_descriptor_it1]);
	EXPECT_EQ(102, network.Get(
				*vertex_descriptor_it1++, *vertex_descriptor_it2++));

	EXPECT_EQ(3, network[*vertex_descriptor_it1]);
	EXPECT_EQ(103, network.Get(
				*vertex_descriptor_it1++, *vertex_descriptor_it2++));

	EXPECT_EQ(4, network[*vertex_descriptor_it1]);
	EXPECT_EQ(104, network.Get(
				*vertex_descriptor_it1++, *vertex_descriptor_it2++));

	EXPECT_EQ(5, network[*vertex_descriptor_it1]);

	EXPECT_EQ(6u, network.GetVertexDescriptors().size());
	EXPECT_EQ(5u, network.GetEdgeDescriptors().size());
}


TEST_F(NetworkTest, VertexDescriptors) {
	EXPECT_EQ(6, std::distance(network.GetVertexDescriptors().begin(),
							   network.GetVertexDescriptors().end()));

	EXPECT_EQ(6u, network.GetVertexDescriptors().size());

	auto it = network.GetVertexDescriptors().begin();
	EXPECT_NE(network.GetVertexDescriptors().end(), it++);
	EXPECT_NE(network.GetVertexDescriptors().end(), it++);
	EXPECT_NE(network.GetVertexDescriptors().end(), it++);
	EXPECT_NE(network.GetVertexDescriptors().end(), it++);
	EXPECT_NE(network.GetVertexDescriptors().end(), it++);
	EXPECT_NE(network.GetVertexDescriptors().end(), it++);
	EXPECT_EQ(network.GetVertexDescriptors().end(), it);
}


TEST_F(NetworkTest, EdgeDescriptors) {
	EXPECT_EQ(5, std::distance(network.GetEdgeDescriptors().begin(),
							   network.GetEdgeDescriptors().end()));
	EXPECT_EQ(5u, network.GetEdgeDescriptors().size());

	auto it = network.GetEdgeDescriptors().begin();
	EXPECT_NE(network.GetEdgeDescriptors().end(), it++);
	EXPECT_NE(network.GetEdgeDescriptors().end(), it++);
	EXPECT_NE(network.GetEdgeDescriptors().end(), it++);
	EXPECT_NE(network.GetEdgeDescriptors().end(), it++);
	EXPECT_NE(network.GetEdgeDescriptors().end(), it++);
	EXPECT_EQ(network.GetEdgeDescriptors().end(), it);
}


TEST_F(NetworkTest, VertexValues) {
	EXPECT_EQ(6, std::distance(network.GetVertexValues().begin(),
							   network.GetVertexValues().end()));

	EXPECT_EQ(6u, network.GetVertexValues().size());

	auto it = network.GetVertexValues().begin();
	EXPECT_EQ(0, *it++);
	EXPECT_EQ(1, *it++);
	EXPECT_EQ(2, *it++);
	EXPECT_EQ(3, *it++);
	EXPECT_EQ(4, *it++);
	EXPECT_EQ(5, *it++);
	EXPECT_EQ(network.GetVertexValues().end(), it);
}


TEST_F(NetworkTest, EdgeValues) {
	EXPECT_EQ(5, std::distance(network.GetEdgeValues().begin(),
							   network.GetEdgeValues().end()));

	EXPECT_EQ(5u, network.GetEdgeValues().size());

	auto it = network.GetEdgeValues().begin();
	EXPECT_EQ(100, *it++);
	EXPECT_EQ(101, *it++);
	EXPECT_EQ(102, *it++);
	EXPECT_EQ(103, *it++);
	EXPECT_EQ(104, *it++);
	EXPECT_EQ(network.GetEdgeValues().end(), it);
}
