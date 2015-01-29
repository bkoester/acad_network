#include "course.hpp"

#include <sstream>
#include <string>

#include "gtest/gtest.h"


using std::string;
using std::stringstream;


class CourseTest : public ::testing::Test {
 public:
	CourseTest() : course2{"ENGLISH", 125}, course3{"EECS", 381},
		undefined_subject{Course::undefined_subject},
		undefined_number{Course::undefined_number} {}

 protected:
	Course course1, course2, course3;
	const string undefined_subject{Course::undefined_subject};
	const int undefined_number{Course::undefined_number};
};


TEST_F(CourseTest, Construction) {
	EXPECT_EQ(undefined_subject, course1.subject());
	EXPECT_EQ(undefined_number, course1.number());

	EXPECT_EQ("ENGLISH", course2.subject());
	EXPECT_EQ(125, course2.number());

	EXPECT_EQ("EECS", course3.subject());
	EXPECT_EQ(381, course3.number());
}


TEST_F(CourseTest, Equality) {
	Course default_course{undefined_subject, undefined_number};
	EXPECT_EQ(default_course, course1);
	EXPECT_EQ(Course("ENGLISH", 125), course2);
	EXPECT_EQ(Course("EECS", 381), course3);

	EXPECT_NE(course1, course2);
	EXPECT_NE(course2, course3);
	EXPECT_NE(course1, course3);
}


TEST_F(CourseTest, Ordering) {
	EXPECT_LT(course1, course2);
	EXPECT_LT(course1, course3);
	EXPECT_LT(course3, course2);
	EXPECT_LT(Course("ENGLISH", 9), course2);
}


TEST_F(CourseTest, Input) {
	stringstream input_stream{
		"PHYSICS\t260\t\t\tlots of stuff\n"
		"  \tNURS\t220    \tmore stuff\t\t\twhat\n"
	};

	Course input_course1, input_course2;
	input_stream >> input_course1 >> input_course2;
	EXPECT_EQ(Course("PHYSICS", 260), input_course1);
	EXPECT_EQ(Course("NURS", 220), input_course2);
}
