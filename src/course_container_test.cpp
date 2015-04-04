#include "course_container.hpp"

#include <algorithm>
#include <sstream>
#include <string>

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

TEST(CourseContainerTest, Find) {
	const MockStudentContainer students{};
	Student student1{147195};
	Student student2{312995};
	Student student3{352468};
	Student student4{500928};
	Student student5{567890};

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
	CourseContainer courses{enrollment_stream, students};

	EXPECT_EQ(6u, courses.size());

	EXPECT_NO_THROW(courses.Find(Course{"ENGLISH", short{125}, 201403}));
	EXPECT_NO_THROW(courses.Find(Course{"CHEM", short{210}, 201403}));
	EXPECT_NO_THROW(courses.Find(Course{"CHEM", short{211}, 201407}));
	EXPECT_NO_THROW(courses.Find(Course{"AAPTIS", short{277}, 201403}));
	EXPECT_NO_THROW(courses.Find(Course{"ENVIRON", short{311}, 201405}));
	EXPECT_NO_THROW(courses.Find(Course{"MATH", short{425}, 201407}));

	Course* english125{courses.Find(
			Course{"ENGLISH", short{125}, 201403}).get()};
	Course* chem210{courses.Find(Course{"CHEM", short{210}, 201403}).get()};
	Course* chem211{courses.Find(Course{"CHEM", short{211}, 201407}).get()};
	Course* aaptis277{courses.Find(
			Course{"AAPTIS", short{277}, 201403}).get()};
	Course* environ311{courses.Find(
			Course{"ENVIRON", short{311}, 201405}).get()};
	Course* math425{courses.Find(Course{"MATH", short{425}, 201407}).get()};

	EXPECT_TRUE(english125->IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(english125->IsStudentEnrolled(student2.id()));
	EXPECT_TRUE(english125->IsStudentEnrolled(student3.id()));
	EXPECT_TRUE(english125->IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(english125->IsStudentEnrolled(student5.id()));

	EXPECT_FALSE(chem210->IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(chem210->IsStudentEnrolled(student2.id()));
	EXPECT_FALSE(chem210->IsStudentEnrolled(student3.id()));
	EXPECT_TRUE(chem210->IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(chem210->IsStudentEnrolled(student5.id()));

	EXPECT_FALSE(chem211->IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(chem211->IsStudentEnrolled(student2.id()));
	EXPECT_FALSE(chem211->IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(chem211->IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(chem211->IsStudentEnrolled(student5.id()));

	EXPECT_TRUE(aaptis277->IsStudentEnrolled(student1.id()));
	EXPECT_TRUE(aaptis277->IsStudentEnrolled(student2.id()));
	EXPECT_FALSE(aaptis277->IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(aaptis277->IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(aaptis277->IsStudentEnrolled(student5.id()));

	EXPECT_TRUE(environ311->IsStudentEnrolled(student1.id()));
	EXPECT_FALSE(environ311->IsStudentEnrolled(student2.id()));
	EXPECT_TRUE(environ311->IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(environ311->IsStudentEnrolled(student4.id()));
	EXPECT_FALSE(environ311->IsStudentEnrolled(student5.id()));

	EXPECT_FALSE(math425->IsStudentEnrolled(student1.id()));
	EXPECT_FALSE(math425->IsStudentEnrolled(student2.id()));
	EXPECT_TRUE(math425->IsStudentEnrolled(student3.id()));
	EXPECT_FALSE(math425->IsStudentEnrolled(student4.id()));
	EXPECT_TRUE(math425->IsStudentEnrolled(student5.id()));
}
