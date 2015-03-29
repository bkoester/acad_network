#include "student_segmentation.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "gtest/gtest.h"

#include "graph_builder.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"
#include "tab_reader.hpp"


using std::begin;
using std::ostream;
using std::ostringstream; using std::stringstream; 
using std::move;
using std::string;


class StudentSegmentationTest : public ::testing::Test {
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
	Student::container_t students;
	Course::container_t enrollment;
	StudentNetwork network;
	StudentSegmenter segmentation;
};


TEST_F(StudentSegmentationTest, VertexData) {
	ASSERT_LT(0u, network.GetVertexDescriptors().size());

	StudentSegmenter::VertexData vertex_data1{
		*begin(network.GetVertexDescriptors()), network, students};
	Student first_student{147195};
	EXPECT_EQ(first_student, vertex_data1.student);
	EXPECT_DOUBLE_EQ(7.0, vertex_data1.weighted_degree);
	EXPECT_EQ(3, vertex_data1.unweighted_degree);
	EXPECT_DOUBLE_EQ(double(7)/12, vertex_data1.weighted_degree_norm);
}

TEST_F(StudentSegmentationTest, RunSegmentation) {
	ostringstream out1;
	ostringstream out2;

	segmentation.AddSegment(out1,
		[](const Student&) { return "segment1"; },
		[](const StudentSegmenter::VertexData&) { return "datum1"; });
	segmentation.AddSegment(out2,
		[](const Student&) { return "segment2"; },
		[](const StudentSegmenter::VertexData&) { return "datum2"; });

	segmentation.RunSegmentation(network, students);

	string result1{out1.str()};
	string result2{out2.str()};

	EXPECT_EQ("segment1\tdatum1\nsegment1\tdatum1\nsegment1\tdatum1\n"
			  "segment1\tdatum1\nsegment1\tdatum1\n", result1);
	EXPECT_EQ("segment2\tdatum2\nsegment2\tdatum2\nsegment2\tdatum2\n"
			  "segment2\tdatum2\nsegment2\tdatum2\n", result2);
}
