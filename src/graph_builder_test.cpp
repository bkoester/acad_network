#include "graph_builder.hpp"

#include <sstream>

#include "gtest/gtest.h"

#include "course.hpp"
#include "network_structure_test.hpp"
#include "student.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"


using std::stringstream;


class GraphBuilderTest : public ::testing::Test {
 public:
	GraphBuilderTest() : enrollment_stream{enrollment_tab} {}

 protected:
	stringstream enrollment_stream;
};


TEST_F(GraphBuilderTest, BuildStudentNetworkFromStudents) { 
	// make the students
	auto course1 = Course{"ENGLISH", 125, 0, 4};
	auto course2 = Course{"AAPTIS", 277, 0, 4};
	auto course3 = Course{"CHEM", 210, 0, 4};
	auto course4 = Course{"CHEM", 211, 0, 1};
	auto course5 = Course{"ENVIRON", 311, 0, 4};
	auto course6 = Course{"MATH", 425, 0, 3};

	Student student1{147195};
	student1.AddCoursesTaken({&course5, &course2, &course1});
	Student student2{312995};
	student2.AddCoursesTaken({&course1, &course3, &course4, &course2});
	Student student3{352468};
	student3.AddCoursesTaken({&course1, &course5, &course6});
	Student student4{500928};
	student4.AddCoursesTaken({&course1, &course3});
	Student student5{567890};
	student5.AddCourseTaken(&course6);
	// put them in a container
	student_container_t students{
		student1, student2, student3, student4, student5};

	course1.AddStudentEnrolled(&students[0]);
	course1.AddStudentEnrolled(&students[1]);
	course1.AddStudentEnrolled(&students[2]);
	course1.AddStudentEnrolled(&students[3]);
	course2.AddStudentEnrolled(&students[0]);
	course2.AddStudentEnrolled(&students[1]);
	course3.AddStudentEnrolled(&students[1]);
	course3.AddStudentEnrolled(&students[3]);
	course4.AddStudentEnrolled(&students[1]);
	course5.AddStudentEnrolled(&students[0]);
	course5.AddStudentEnrolled(&students[2]);
	course6.AddStudentEnrolled(&students[2]);
	course6.AddStudentEnrolled(&students[4]);


	StudentNetwork network{BuildStudentNetworkFromStudents(students)};
	TestStudentNetworkStructure(network);
}
