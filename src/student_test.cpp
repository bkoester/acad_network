#include "student.hpp"

#include <sstream>
#include <memory>

#include "gtest/gtest.h"

#include "course.hpp"


using std::stringstream;
using std::unique_ptr;


class StudentTest : public ::testing::Test {
 public:
	StudentTest() : 
		student2{2}, student3{3}, uninitialized_id{Student::uninitialized_id} {}

 protected:
	Student student1, student2, student3;	
	const int uninitialized_id;
};


TEST_F(StudentTest, Construction) {
	EXPECT_EQ(uninitialized_id, student1.id());
	EXPECT_EQ(2, student2.id());
	EXPECT_EQ(3, student3.id());
}


TEST_F(StudentTest, Classes) {
	auto course1 = Course{"ENGLISH", 125, 0};
	auto course2 = Course{"EECS", 381, 0};
	auto course3 = Course{"PHYSICS", 260, 0};

	student1.AddCourseTaken(&course1);
	student1.AddCourseTaken(&course2);
	student1.AddCourseTaken(&course3);

	student2.AddCourseTaken(&course1);
	student2.AddCourseTaken(&course2);

	EXPECT_TRUE(student1.HasTakenCourse(&course1));
	EXPECT_TRUE(student1.HasTakenCourse(&course2));
	EXPECT_TRUE(student1.HasTakenCourse(&course3));

	EXPECT_TRUE(student2.HasTakenCourse(&course1));
	EXPECT_TRUE(student2.HasTakenCourse(&course2));
	EXPECT_FALSE(student2.HasTakenCourse(&course3));

	EXPECT_FALSE(student3.HasTakenCourse(&course1));
	EXPECT_FALSE(student3.HasTakenCourse(&course2));
	EXPECT_FALSE(student3.HasTakenCourse(&course3));
}


TEST_F(StudentTest, Input) {
	stringstream input_stream{
		"123\t\t\t  \t\t\tlots of stuff\n"
		"  \t234    \tmore stuff\t\t\twhat\n"
	};


	Student input_student1, input_student2;
	input_stream >> input_student1 >> input_student2;
	EXPECT_EQ(Student(123), input_student1);
	EXPECT_EQ(Student(234), input_student2);
}
