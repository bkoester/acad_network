#include "student.hpp"

#include <memory>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/optional.hpp>
#include "gtest/gtest.h"

#include "course.hpp"


using std::stringstream;
using std::unique_ptr;

using boost::make_optional;


class StudentTest : public ::testing::Test {
 public:
	StudentTest() : 
		uninitialized_id{Student::uninitialized_id},
		uninitialized_term{Student::uninitialized_term},
		unused_id{0},
		student1{1, Student::Gender::Female, Student::Ethnicity::Asian, 
			201107, 201503, true, "ULSA"},
		student2{2, Student::Gender::Male, Student::Ethnicity::White, 
			201201, uninitialized_term, false, "UKIN"},
		student3{3, Student::Gender::Unspecified, Student::Ethnicity::Unknown, 
			uninitialized_term, uninitialized_term, false, "UENG"} {}


	void SetUp() override {
		student1.major1_ = boost::make_optional<double>(141001);
		student1.major2_ = boost::make_optional<double>(160102);
		student2.major1_ = boost::make_optional<double>(1612);
	}

 protected:
	const int uninitialized_id;
	const int uninitialized_term;
	const int unused_id;
	Student student1, student2, student3;	
};


TEST_F(StudentTest, Construction) {
	EXPECT_EQ(1, student1.id());
	EXPECT_EQ(2, student2.id());
	EXPECT_EQ(3, student3.id());

	EXPECT_EQ(201107, student1.first_term());
	EXPECT_EQ(Student::Gender::Female, student1.gender());
	EXPECT_EQ(Student::Ethnicity::Asian, student1.ethnicity());
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

	Student input_student1;
	input_stream >> input_student1;
	// copy because istream_iterator does it and operator>> should reset all
	// properties not used.
	Student input_student2{input_student1};
	input_stream >> input_student2;

	EXPECT_EQ(Student(123), input_student1);
	EXPECT_EQ(123, input_student1.id());
	EXPECT_EQ(Student::Gender::Female, input_student1.gender());
	EXPECT_EQ(Student::Ethnicity::Black, input_student1.ethnicity());
	EXPECT_EQ(201107, input_student1.first_term());
	EXPECT_TRUE(bool(input_student1.major1()));
	EXPECT_EQ(422706, input_student1.major1().get());
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


TEST_F(StudentTest, Serialize) {
	stringstream student_stream;
	boost::archive::text_oarchive student_oarchive{student_stream};

	student1.serialize(student_oarchive, 0);
	student2.serialize(student_oarchive, 0);
	student3.serialize(student_oarchive, 0);

	boost::archive::text_iarchive student_iarchive{student_stream};

	Student serialized_student1, serialized_student2, serialized_student3;
	serialized_student1.serialize(student_iarchive, 0);
	serialized_student2.serialize(student_iarchive, 0);
	serialized_student3.serialize(student_iarchive, 0);

	EXPECT_EQ(1, serialized_student1.id());
	EXPECT_EQ(2, serialized_student2.id());
	EXPECT_EQ(3, serialized_student3.id());

	EXPECT_EQ(Student::Gender::Female, serialized_student1.gender());
	EXPECT_EQ(Student::Gender::Male, serialized_student2.gender());
	EXPECT_EQ(Student::Gender::Unspecified, serialized_student3.gender());

	EXPECT_EQ(Student::Ethnicity::Asian, serialized_student1.ethnicity());
	EXPECT_EQ(Student::Ethnicity::White, serialized_student2.ethnicity());
	EXPECT_EQ(Student::Ethnicity::Unknown, serialized_student3.ethnicity());

	EXPECT_EQ(201107, serialized_student1.first_term());
	EXPECT_EQ(201201, serialized_student2.first_term());
	EXPECT_EQ(uninitialized_term, serialized_student3.first_term());

	EXPECT_EQ(201503, serialized_student1.degree_term());
	EXPECT_EQ(uninitialized_term, serialized_student2.degree_term());
	EXPECT_EQ(uninitialized_term, serialized_student3.degree_term());

	EXPECT_TRUE(serialized_student1.transfer());
	EXPECT_FALSE(serialized_student2.transfer());
	EXPECT_FALSE(serialized_student3.transfer());

	EXPECT_EQ("ULSA", serialized_student1.school());
	EXPECT_EQ("UKIN", serialized_student2.school());
	EXPECT_EQ("UENG", serialized_student3.school());

	EXPECT_EQ(141001, serialized_student1.major1().get());
	EXPECT_EQ(1612, serialized_student2.major1().get());
	EXPECT_FALSE(bool(serialized_student3.major1()));

	EXPECT_EQ(160102, serialized_student1.major2().get());
	EXPECT_FALSE(bool(serialized_student2.major2()));
	EXPECT_FALSE(bool(serialized_student3.major2()));
}
