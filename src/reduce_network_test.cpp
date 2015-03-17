#include "reduce_network.hpp"

#include <functional>
#include <iterator>
#include <sstream>

#include "gtest/gtest.h"

#include "graph_builder.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"
#include "tab_reader.hpp"


using std::begin;
using std::plus;
using std::stringstream;


class ReduceNetworkTest : public ::testing::Test {
 public:
	void SetUp() override {
		// yes, this should be mocked, but mocking is a lot of work
		stringstream student_stream{student_tab};
		students = ReadStudents(student_stream);
		stringstream enrollment_stream{enrollment_tab};
		enrollment = ReadEnrollment(enrollment_stream, students);
		network = BuildStudentNetworkFromStudents(students);
	}
 protected:
	student_container_t students;
	course_container_t enrollment;
	StudentNetwork network;
};

TEST_F(ReduceNetworkTest, ReduceNework) {
	auto weighted_gender_network = ReduceNetwork(network,
			[this](const StudentId& id)
			{ return FindStudent(id, students).gender(); },
			plus<double>{}, 0.);
	auto unweighted_gender_network = ReduceNetwork(network,
			[this](const StudentId& id)
			{ return FindStudent(id, students).gender(); },
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
