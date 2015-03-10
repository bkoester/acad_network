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
		student2{2}, student3{3}, uninitialized_id{Student::uninitialized_id},
		uninitialized_first_term{Student::uninitialized_first_term} {}

 protected:
	Student student1, student2, student3;	
	const int uninitialized_id;
	const int uninitialized_first_term;
};


TEST_F(StudentTest, Construction) {
	EXPECT_EQ(uninitialized_id, student1.id());
	EXPECT_EQ(2, student2.id());
	EXPECT_EQ(3, student3.id());

	EXPECT_EQ(uninitialized_first_term, student1.first_term());
	EXPECT_EQ(Student::Gender::Unspecified, student1.gender());
	EXPECT_EQ(Student::Ethnicity::Unknown, student1.ethnicity());
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


TEST_F(StudentTest, GetTotalCreditsTaken) {
	auto course1 = Course{"ENGLISH", 125, 0, 3};
	auto course2 = Course{"EECS", 381, 0, 4};
	auto course3 = Course{"PHYSICS", 260, 0, 1.5};

	student1.AddCourseTaken(&course1);
	student1.AddCourseTaken(&course2);
	student1.AddCourseTaken(&course3);

	student3.AddCourseTaken(&course3);

	EXPECT_EQ(8.5, student1.GetTotalCreditsTaken());
	EXPECT_EQ(0., student2.GetTotalCreditsTaken());
	EXPECT_EQ(1.5, student3.GetTotalCreditsTaken());
}


TEST_F(StudentTest, Input) {
	stringstream input_stream{
		"123\t\t\t  F  \t\t\t4    201107     201303  Y\t42.2706\tNA\t"
		"Physiological Psychology/Psych\tNA\tlol\tw/e\tduncare\tN\t24\t25\t24\t"
		"ULSA\n"
		"  \t234    \tM      6 201303\t\t\t201403  N\tNA\tNA\tNA\tNA\tNA\tNA\t"
		"NA\tN\t31\t34\t31\tUARC\n"
	};

	Student input_student1, input_student2;
	input_stream >> input_student1 >> input_student2;

	EXPECT_EQ(Student(123), input_student1);
	EXPECT_EQ(123, input_student1.id());
	EXPECT_EQ(Student::Gender::Female, input_student1.gender());
	EXPECT_EQ(Student::Ethnicity::Black, input_student1.ethnicity());
	EXPECT_EQ(201107, input_student1.first_term());
	EXPECT_TRUE(bool(input_student1.major1()));
	EXPECT_EQ(42.2706, input_student1.major1().get());
	EXPECT_FALSE(bool(input_student1.major2()));
	EXPECT_EQ("Physiological Psychology/Psychology",
			  input_student1.GetMajor1Description());
	EXPECT_EQ("NA", input_student1.GetMajor2Description());
	EXPECT_EQ("ULSA", input_student1.school());

	EXPECT_EQ(Student(234), input_student2);
	EXPECT_EQ(234, input_student2.id());
	EXPECT_EQ(Student::Gender::Male, input_student2.gender());
	EXPECT_EQ(Student::Ethnicity::White, input_student2.ethnicity());
	EXPECT_EQ(201303, input_student2.first_term());
	EXPECT_FALSE(bool(input_student2.major1()));
	EXPECT_FALSE(bool(input_student2.major2()));
	EXPECT_EQ("NA", input_student2.GetMajor1Description());
	EXPECT_EQ("NA", input_student2.GetMajor2Description());
	EXPECT_EQ("UARC", input_student2.school());
}
