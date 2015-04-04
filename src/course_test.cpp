#include "course.hpp"

#include <sstream>
#include <string>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "gtest/gtest.h"

#include "student.hpp"


using std::string;
using std::stringstream;
using std::unique_ptr;


class CourseTest : public ::testing::Test {
 public:
	CourseTest() : course2{"ENGLISH", 125, 201403}, course3{"EECS", 381, 201303},
			undefined_subject{Course::undefined_subject},
			undefined_number{Course::undefined_number} {
		course2.num_credits_ = 4;
		course3.num_credits_ = 4;
	}

 protected:
	Course course1, course2, course3;
	const string undefined_subject{Course::undefined_subject};
	const short undefined_number{Course::undefined_number};
	const int undefined_term{Course::undefined_term};
};


TEST_F(CourseTest, Construction) {
	EXPECT_EQ(undefined_subject, course1.subject());
	EXPECT_EQ(undefined_number, course1.number());
	EXPECT_EQ(undefined_term, course1.term());
	EXPECT_EQ(0, course1.num_credits());

	EXPECT_EQ("ENGLISH", course2.subject());
	EXPECT_EQ(125, course2.number());
	EXPECT_EQ(201403, course2.term());
	EXPECT_EQ(4, course2.num_credits());

	EXPECT_EQ("EECS", course3.subject());
	EXPECT_EQ(381, course3.number());
	EXPECT_EQ(201303, course3.term());
	EXPECT_EQ(4, course3.num_credits());
}


TEST_F(CourseTest, Equality) {
	Course default_course{undefined_subject, undefined_number, undefined_term};
	EXPECT_EQ(default_course, course1);
	EXPECT_EQ(Course("ENGLISH", 125, 201403), course2);
	EXPECT_EQ(Course("EECS", 381, 201303), course3);

	EXPECT_NE(Course("EECS", 382, 201403), course3);
	EXPECT_NE(Course("EECS", 381, 201301), course3);

	EXPECT_NE(course1, course2);
	EXPECT_NE(course2, course3);
	EXPECT_NE(course1, course3);
}


TEST_F(CourseTest, Ordering) {
	EXPECT_LT(course1, course2);
	EXPECT_LT(course1, course3);
	EXPECT_LT(course3, course2);
	EXPECT_LT(Course("ENGLISH", 9, 201401), course2);
	EXPECT_LT(Course("ENGLISH", 125, 201401), course2);
}

TEST_F(CourseTest, EnrolledStudents) {
	auto student1 = unique_ptr<Student>{new Student{1}};
	auto student2 = unique_ptr<Student>{new Student{2}};
	auto student3 = unique_ptr<Student>{new Student{3}};

	course1.AddStudentEnrolled(student1->id());
	course1.AddStudentEnrolled(student2->id());
	course2.AddStudentEnrolled(student2->id());
	course2.AddStudentEnrolled(student3->id());
	course3.AddStudentEnrolled(student3->id());

	EXPECT_EQ(2u, course1.GetNumStudentsEnrolled());
	EXPECT_EQ(2u, course2.GetNumStudentsEnrolled());
	EXPECT_EQ(1u, course3.GetNumStudentsEnrolled());
}


TEST_F(CourseTest, Input) {
	stringstream input_stream{
		"PHYSICS\t260\tNA\t4\t3.60454545454545\t3.66538461538462\t26\t95.3"
		"\t4\t201403\n"
		"LHC\t250\tNA\t4\t3.60454545454545\tNA\t26.5\t95.3\t1.5\t201303\n"
	};

	Course input_course1, input_course2;
	input_stream >> input_course1 >> input_course2;
	EXPECT_EQ(Course("PHYSICS", 260, 201403), input_course1);
	EXPECT_EQ(Course("LHC", 250, 201303), input_course2);

	EXPECT_EQ(4, input_course1.num_credits());
	EXPECT_EQ(1.5, input_course2.num_credits());
}


TEST_F(CourseTest, Serialization) {
	course1.AddStudentEnrolled(Student::Id{1});
	course1.AddStudentEnrolled(Student::Id{2});
	course1.AddStudentEnrolled(Student::Id{3});

	course2.AddStudentEnrolled(Student::Id{2});
	course2.AddStudentEnrolled(Student::Id{4});

	stringstream course_stream;
	boost::archive::text_oarchive course_oarchive{course_stream};

	course1.serialize(course_oarchive, 0);
	course2.serialize(course_oarchive, 0);
	course3.serialize(course_oarchive, 0);

	boost::archive::text_iarchive course_iarchive{course_stream};

	Course serialized_course1, serialized_course2, serialized_course3;
	serialized_course1.serialize(course_iarchive, 0);
	serialized_course2.serialize(course_iarchive, 0);
	serialized_course3.serialize(course_iarchive, 0);

	EXPECT_EQ(undefined_subject, serialized_course1.subject());
	EXPECT_EQ("ENGLISH", serialized_course2.subject());
	EXPECT_EQ("EECS", serialized_course3.subject());

	EXPECT_EQ(undefined_number, serialized_course1.number());
	EXPECT_EQ(125, serialized_course2.number());
	EXPECT_EQ(381, serialized_course3.number());

	EXPECT_EQ(undefined_term, serialized_course1.term());
	EXPECT_EQ(201403, serialized_course2.term());
	EXPECT_EQ(201303, serialized_course3.term());

	EXPECT_EQ(0, serialized_course1.num_credits());
	EXPECT_EQ(4, serialized_course2.num_credits());
	EXPECT_EQ(4, serialized_course3.num_credits());

	EXPECT_EQ(0, serialized_course1.num_credits());
	EXPECT_EQ(4, serialized_course2.num_credits());
	EXPECT_EQ(4, serialized_course3.num_credits());

	EXPECT_EQ(3u, serialized_course1.GetNumStudentsEnrolled());
	EXPECT_TRUE(serialized_course1.IsStudentEnrolled(Student::Id{1}));
	EXPECT_TRUE(serialized_course1.IsStudentEnrolled(Student::Id{2}));
	EXPECT_TRUE(serialized_course1.IsStudentEnrolled(Student::Id{3}));
	EXPECT_FALSE(serialized_course1.IsStudentEnrolled(Student::Id{4}));
	
	EXPECT_EQ(2u, serialized_course2.GetNumStudentsEnrolled());
	EXPECT_TRUE(serialized_course2.IsStudentEnrolled(Student::Id{2}));
	EXPECT_TRUE(serialized_course2.IsStudentEnrolled(Student::Id{4}));
	EXPECT_FALSE(serialized_course2.IsStudentEnrolled(Student::Id{1}));

	EXPECT_EQ(0u, serialized_course3.GetNumStudentsEnrolled());
	EXPECT_FALSE(serialized_course2.IsStudentEnrolled(Student::Id{1}));
}
