#include "weighting_function.hpp"

#include <boost/optional.hpp>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"


using boost::optional;
using boost::make_optional;


class WeightingFunctionTest : public ::testing::Test {
 public:
	WeightingFunctionTest() : course1{"ENGLISH", 125, 0, 4},
			course2{"AAPTIS", 277, 0, 4}, course3{"CHEM", 210, 0, 4},
			course4{"CHEM", 211, 0, 1}, course5{"ENVIRON", 311, 0, 4},
			course6{"MATH", 425, 0, 3}, student1{1}, student2{2}, student3{3},
			student4{4} {
		student1.AddCoursesTaken({&course1, &course2, &course4});
		student2.AddCoursesTaken({&course2, &course3});
		student3.AddCoursesTaken({&course1, &course3, &course5});
		student4.AddCoursesTaken({&course3, &course5, &course6});

		course1.AddStudentEnrolled(student1.id());
		course1.AddStudentEnrolled(student3.id());
		course2.AddStudentEnrolled(student1.id());
		course2.AddStudentEnrolled(student2.id());
		course3.AddStudentEnrolled(student2.id());
		course3.AddStudentEnrolled(student3.id());
		course3.AddStudentEnrolled(student4.id());
		course4.AddStudentEnrolled(student1.id());
		course5.AddStudentEnrolled(student3.id());
		course5.AddStudentEnrolled(student4.id());
		course6.AddStudentEnrolled(student4.id());
	}

 protected:
	Course course1, course2, course3, course4, course5, course6;
	Student student1, student2, student3, student4;
};


TEST_F(WeightingFunctionTest, CreditHoursOverEnrollment) {
	EXPECT_DOUBLE_EQ(5., CreditHoursOverEnrollment(student1, student1).value());
	EXPECT_DOUBLE_EQ(2., CreditHoursOverEnrollment(student1, student2).value());
	EXPECT_DOUBLE_EQ(2., CreditHoursOverEnrollment(student1, student3).value());
	EXPECT_FALSE(
			static_cast<bool>(CreditHoursOverEnrollment(student1, student4)));
	EXPECT_FALSE(
			static_cast<bool>(CreditHoursOverEnrollment(student1, student4)));
	EXPECT_DOUBLE_EQ(10.0/3,
					 CreditHoursOverEnrollment(student2, student2).value());
	EXPECT_DOUBLE_EQ(4.0/3,
					 CreditHoursOverEnrollment(student2, student3).value());
	EXPECT_DOUBLE_EQ(4.0/3,
					 CreditHoursOverEnrollment(student2, student4).value());
	EXPECT_DOUBLE_EQ(16.0/3,
					 CreditHoursOverEnrollment(student3, student3).value());
	EXPECT_DOUBLE_EQ(10.0/3,
					 CreditHoursOverEnrollment(student3, student4).value());
	EXPECT_DOUBLE_EQ(19.0/3,
					 CreditHoursOverEnrollment(student4, student4).value());
}

TEST_F(WeightingFunctionTest, InverseEnrollment) {
	EXPECT_DOUBLE_EQ(2., InverseEnrollment(student1, student1).value());
	EXPECT_DOUBLE_EQ(0.5, InverseEnrollment(student1, student2).value());
	EXPECT_DOUBLE_EQ(0.5, InverseEnrollment(student1, student3).value());
	EXPECT_FALSE(static_cast<bool>(InverseEnrollment(student1, student4)));
	EXPECT_FALSE(static_cast<bool>(InverseEnrollment(student1, student4)));
	EXPECT_DOUBLE_EQ(5.0/6, InverseEnrollment(student2, student2).value());
	EXPECT_DOUBLE_EQ(1.0/3, InverseEnrollment(student2, student3).value());
	EXPECT_DOUBLE_EQ(1.0/3, InverseEnrollment(student2, student4).value());
	EXPECT_DOUBLE_EQ(4.0/3, InverseEnrollment(student3, student3).value());
	EXPECT_DOUBLE_EQ(5.0/6, InverseEnrollment(student3, student4).value());
	EXPECT_DOUBLE_EQ(11.0/6, InverseEnrollment(student4, student4).value());
}
