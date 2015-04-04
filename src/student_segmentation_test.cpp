#include "student_segmentation.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "course_container.hpp"
#include "graph_builder.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"


using std::begin;
using std::ostream;
using std::ostringstream; using std::stringstream; 
using std::move;
using std::string;
using std::vector;


class StudentSegmentationTest : public ::testing::Test {
 public:
	void SetUp() override {
		StudentNetwork::graph_t graph{5};

		auto course1 = Course{"ENGLISH", 125, 0, 4};
		auto course2 = Course{"AAPTIS", 277, 0, 4};
		auto course3 = Course{"CHEM", 210, 0, 4};
		auto course4 = Course{"CHEM", 211, 0, 1};
		auto course5 = Course{"ENVIRON", 311, 0, 4};
		auto course6 = Course{"MATH", 425, 0, 3};
		courses = {course1, course2, course3, course4, course5, course6};

		Student student1{147195, Student::Gender::Male, 
			Student::Ethnicity::Unknown, 0, 0, false, ""};
		student1.AddCoursesTaken({&courses[4], &courses[1], &courses[0]});
		Student student2{312995, Student::Gender::Female, 
			Student::Ethnicity::Unknown, 0, 0, false, ""};
		student2.AddCoursesTaken(
				{&courses[0], &courses[2], &courses[3], &courses[1]});
		Student student3{352468, Student::Gender::Female, 
			Student::Ethnicity::Unknown, 0, 0, false, ""};
		student3.AddCoursesTaken({&courses[0], &courses[4], &courses[5]});
		Student student4{500928, Student::Gender::Female, 
			Student::Ethnicity::Unknown, 0, 0, false, ""};
		student4.AddCoursesTaken({&courses[0], &courses[2]});
		Student student5{567890, Student::Gender::Male, 
			Student::Ethnicity::Unknown, 0, 0, false, ""};
		student5.AddCourseTaken(&courses[5]);

		// add vertices
		auto student1_v = vertex(0, graph);
		graph[student1_v] = student1.id();
		auto student2_v = vertex(1, graph);
		graph[student2_v] = student2.id();
		auto student3_v = vertex(2, graph);
		graph[student3_v] = student3.id();
		auto student4_v = vertex(3, graph);
		graph[student4_v] = student4.id();
		auto student5_v = vertex(4, graph);
		graph[student5_v] = student5.id();

		// add edges
		add_edge(student1_v, student2_v, 3.0, graph);
		add_edge(student1_v, student3_v, 3.0, graph);
		add_edge(student1_v, student4_v, 1.0, graph);

		add_edge(student2_v, student3_v, 1.0, graph);
		add_edge(student2_v, student4_v, 1.0, graph);

		add_edge(student3_v, student4_v, 3.0, graph);
		add_edge(student4_v, student5_v, 1.5, graph);

		network = StudentNetwork{graph};

		students.Insert({student1, student2, student3, student4, student5});
	}

 protected:
	vector<Course> courses;
	StudentContainer students;
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
