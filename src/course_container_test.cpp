#include "course_container.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"
#include "utility.hpp"
#include "student_container_mock.hpp"


using std::binary_search;
using std::string;
using std::stringstream;

using ::testing::ReturnRef;
using ::testing::Eq;
using ::testing::AtLeast;


class CourseContainerTest : public ::testing::Test {
 public:
	CourseContainerTest() :
		student1{147195}, student2{312995}, student3{352468}, student4{500928},
		student5{567890} {}

	void SetUp() override {
		EXPECT_CALL(students, Find(147195)).Times(AtLeast(1)).
			WillRepeatedly(ReturnRef(student1));
		EXPECT_CALL(students, Find(312995)).Times(AtLeast(1)).
			WillRepeatedly(ReturnRef(student2));
		EXPECT_CALL(students, Find(352468)).Times(AtLeast(1)).
			WillRepeatedly(ReturnRef(student3));
		EXPECT_CALL(students, Find(500928)).Times(AtLeast(1)).
			WillRepeatedly(ReturnRef(student4));
		EXPECT_CALL(students, Find(567890)).Times(AtLeast(1)).
			WillRepeatedly(ReturnRef(student5));

		stringstream enrollment_stream{enrollment_tab};
		courses = CourseContainer{enrollment_stream, students};
	}


 protected:
	const MockStudentContainer students;
	Student student1, student2, student3, student4, student5;
	CourseContainer courses;
};


TEST_F(CourseContainerTest, Find) {
	EXPECT_EQ(6u, courses.size());

	EXPECT_NO_THROW(courses.Find(Course{"ENGLISH", short{125}, 201403}));
	EXPECT_NO_THROW(courses.Find(Course{"CHEM", short{210}, 201403}));
	EXPECT_NO_THROW(courses.Find(Course{"CHEM", short{211}, 201407}));
	EXPECT_NO_THROW(courses.Find(Course{"AAPTIS", short{277}, 201403}));
	EXPECT_NO_THROW(courses.Find(Course{"ENVIRON", short{311}, 201405}));
	EXPECT_NO_THROW(courses.Find(Course{"MATH", short{425}, 201407}));

	Course& english125{courses.Find(
			Course{"ENGLISH", short{125}, 201403})};
	Course& chem210{courses.Find(Course{"CHEM", short{210}, 201403})};
	Course& chem211{courses.Find(Course{"CHEM", short{211}, 201407})};
	Course& aaptis277{courses.Find(
			Course{"AAPTIS", short{277}, 201403})};
	Course& environ311{courses.Find(
			Course{"ENVIRON", short{311}, 201405})};
	Course& math425{courses.Find(Course{"MATH", short{425}, 201407})};

	EXPECT_TRUE(english125.IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(english125.IsStudentEnrolled(student2.id()));
	EXPECT_TRUE(english125.IsStudentEnrolled(student3.id()));
	EXPECT_TRUE(english125.IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(english125.IsStudentEnrolled(student5.id()));

	EXPECT_FALSE(chem210.IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(chem210.IsStudentEnrolled(student2.id()));
	EXPECT_FALSE(chem210.IsStudentEnrolled(student3.id()));
	EXPECT_TRUE(chem210.IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(chem210.IsStudentEnrolled(student5.id()));

	EXPECT_FALSE(chem211.IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(chem211.IsStudentEnrolled(student2.id()));
	EXPECT_FALSE(chem211.IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(chem211.IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(chem211.IsStudentEnrolled(student5.id()));

	EXPECT_TRUE(aaptis277.IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(aaptis277.IsStudentEnrolled(student2.id()));
	EXPECT_FALSE(aaptis277.IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(aaptis277.IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(aaptis277.IsStudentEnrolled(student5.id()));

	EXPECT_TRUE(environ311.IsStudentEnrolled(student1.id()));
	EXPECT_FALSE(environ311.IsStudentEnrolled(student2.id()));
	EXPECT_TRUE(environ311.IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(environ311.IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(environ311.IsStudentEnrolled(student5.id()));

	EXPECT_FALSE(math425.IsStudentEnrolled(student1.id()));
	EXPECT_FALSE(math425.IsStudentEnrolled(student2.id()));
	EXPECT_TRUE(math425.IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(math425.IsStudentEnrolled(student4.id()));
	EXPECT_TRUE(math425.IsStudentEnrolled(student5.id()));
}


TEST_F(CourseContainerTest, Serialization) {
	stringstream course_stream;
	courses.Save(course_stream);

	CourseContainer serialized_courses;
	serialized_courses.Load(course_stream);

	EXPECT_EQ(courses, serialized_courses);
}
