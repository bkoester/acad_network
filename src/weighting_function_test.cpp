#include "weighting_function.hpp"

#include <boost/optional.hpp>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"


using boost::optional;
using boost::make_optional;


TEST(WeightingFunctionTest, CreditHoursOverEnrollment) {
	auto course1 = Course{"ENGLISH", 125, 0, 4};
	auto course2 = Course{"AAPTIS", 277, 0, 4};
	auto course3 = Course{"CHEM", 210, 0, 4};
	auto course4 = Course{"CHEM", 211, 0, 1};
	auto course5 = Course{"ENVIRON", 311, 0, 4};
	auto course6 = Course{"MATH", 425, 0, 3};

	auto student1 = Student{1};
	student1.AddCoursesTaken({&course1, &course2, &course4});
	auto student2 = Student{2};
	student2.AddCoursesTaken({&course2, &course3});
	auto student3 = Student{3};
	student3.AddCoursesTaken({&course1, &course3, &course5});
	auto student4 = Student{4};
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

	EXPECT_DOUBLE_EQ(make_optional<double>(5.).value(),
					 CreditHoursOverEnrollment(student1, student1).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(2.).value(),
					 CreditHoursOverEnrollment(student1, student2).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(2.).value(),
					 CreditHoursOverEnrollment(student1, student3).value());
	EXPECT_FALSE(
			static_cast<bool>(CreditHoursOverEnrollment(student1, student4)));
	EXPECT_FALSE(
			static_cast<bool>(CreditHoursOverEnrollment(student1, student4)));
	EXPECT_DOUBLE_EQ(make_optional<double>(10.0/3).value(),
					 CreditHoursOverEnrollment(student2, student2).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(4.0/3).value(),
					 CreditHoursOverEnrollment(student2, student3).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(4.0/3).value(),
					 CreditHoursOverEnrollment(student2, student4).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(16.0/3).value(),
					 CreditHoursOverEnrollment(student3, student3).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(10.0/3).value(),
					 CreditHoursOverEnrollment(student3, student4).value());
	EXPECT_DOUBLE_EQ(make_optional<double>(19.0/3).value(),
					 CreditHoursOverEnrollment(student4, student4).value());
}
