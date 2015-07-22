#include "reduce_network.hpp"

#include <functional>
#include <iterator>
#include <set>
#include <sstream>

#include "gtest/gtest.h"

#include "student_container.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"


using std::begin;
using std::plus;
using std::stringstream;
using std::set;


class ReduceNetworkTest : public ::testing::Test {
 public:
	void SetUp() override {
		StudentNetwork::graph_t graph{5};

		// add vertices
		auto student1 = vertex(0, graph);
		graph[student1] = Student::Id{312995};
		auto student2 = vertex(1, graph);
		graph[student2] = Student::Id{500928};
		auto student3 = vertex(2, graph);
		graph[student3] = Student::Id{147195};
		auto student4 = vertex(3, graph);
		graph[student4] = Student::Id{352468};
		auto student5 = vertex(4, graph);
		graph[student5] = Student::Id{567890};

		// add edges
		add_edge(student1, student2, 3.0, graph);
		add_edge(student1, student3, 3.0, graph);
		add_edge(student1, student4, 1.0, graph);

		add_edge(student2, student3, 1.0, graph);
		add_edge(student2, student4, 1.0, graph);

		add_edge(student3, student4, 3.0, graph);
		add_edge(student4, student5, 1.5, graph);

		network = StudentNetwork{graph};

		students.insert({
			Student{147195, Student::Gender::Male, Student::Ethnicity::Unknown,
				0, 0, false, ""},
			Student{312995, Student::Gender::Female, Student::Ethnicity::Unknown,
				0, 0, false, ""},
			Student{352468, Student::Gender::Female, Student::Ethnicity::Unknown,
				0, 0, false, ""},
			Student{500928, Student::Gender::Female, Student::Ethnicity::Unknown,
				0, 0, false, ""},
			Student{567890, Student::Gender::Male, Student::Ethnicity::Unknown,
				0, 0, false, ""}
		});
	}

 protected:
	set<Student> students;
	StudentNetwork network;
};


TEST_F(ReduceNetworkTest, ReduceNework) {
	auto weighted_gender_network = ReduceNetwork(network,
			[this](const Student::Id& id)
			{ return students.find(Student{id})->gender(); },
			plus<double>{}, 0.);
	auto unweighted_gender_network = ReduceNetwork(network,
			[this](const Student::Id& id)
			{ return students.find(Student{id})->gender(); },
			[](double, int old_value) { return 1 + old_value; }, 0);

	// find the vertices for weighted network
	auto male_weighted_vertex = 
		weighted_gender_network.GetVertexDescriptor(Student::Gender::Male);
	auto female_weighted_vertex = 
		weighted_gender_network.GetVertexDescriptor(Student::Gender::Female);

	EXPECT_DOUBLE_EQ(8.5, weighted_gender_network.Get(
				male_weighted_vertex, female_weighted_vertex));
	EXPECT_DOUBLE_EQ(5.0, weighted_gender_network.Get(
				female_weighted_vertex, female_weighted_vertex));
	EXPECT_THROW(weighted_gender_network.Get(
				male_weighted_vertex, male_weighted_vertex), NoEdgeException);

	// find the vertices for unweighted network
	auto male_unweighted_vertex = 
		unweighted_gender_network.GetVertexDescriptor(Student::Gender::Male);
	auto female_unweighted_vertex = 
		unweighted_gender_network.GetVertexDescriptor(Student::Gender::Female);
	
	EXPECT_EQ(4, unweighted_gender_network.Get(
				male_unweighted_vertex, female_unweighted_vertex));
	EXPECT_EQ(3, unweighted_gender_network.Get(
				female_unweighted_vertex, female_unweighted_vertex));
	EXPECT_THROW(unweighted_gender_network.Get(
				male_unweighted_vertex, male_unweighted_vertex), 
			NoEdgeException);
}
