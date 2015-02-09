#include "tab_reader.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"
#include "utility.hpp"


using std::binary_search;
using std::string;
using std::stringstream;


TEST(TabRecordTest, ReadStudents) {
	stringstream student_stream{student_tab};
	auto students = ReadStudents(student_stream);

	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{312995}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{500928}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{147195}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{352468}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{567890}));
	EXPECT_FALSE(binary_search(
				students.begin(), students.end(), Student{111111}));
	EXPECT_FALSE(binary_search(
				students.begin(), students.end(), Student{999999}));
	EXPECT_FALSE(binary_search(
				students.begin(), students.end(), Student{333333}));


	EXPECT_EQ(5u, students.size());
}

TEST(TabReaderTest, ReadEnrollment) {
	student_container_t students{Student{147195}, Student{312995}, 
		Student{352468}, Student{500928}, Student{5678890}};

	stringstream enrollment_stream{enrollment_tab};
	auto courses = ReadEnrollment(enrollment_stream, students);

	EXPECT_NE(end(courses), 
			  courses.find(make_unique<Course>("ENGLISH", short{125}, 201403)));
	EXPECT_NE(end(courses),
			  courses.find(make_unique<Course>("CHEM", short{210}, 201403)));
	EXPECT_NE(end(courses),
			  courses.find(make_unique<Course>("CHEM", short{211}, 201407)));
	EXPECT_NE(end(courses),
			  courses.find(make_unique<Course>("AAPTIS", short{277}, 201403)));
	EXPECT_NE(end(courses),
			  courses.find(make_unique<Course>("ENVIRON", short{311}, 201405)));
	EXPECT_NE(end(courses),
			  courses.find(make_unique<Course>("MATH", short{425}, 201407)));

	Course* english125{courses.find(
			make_unique<Course>("ENGLISH", short{125}, 201403))->get()};
	Course* chem210{courses.find(
			make_unique<Course>("CHEM", short{210}, 201403))->get()};
	Course* chem211{courses.find(
			make_unique<Course>("CHEM", short{211}, 201407))->get()};
	Course* aaptis277{courses.find(
			make_unique<Course>("AAPTIS", short{277}, 201403))->get()};
	Course* environ311{courses.find(
			make_unique<Course>("ENVIRON", short{311}, 201405))->get()};
	Course* math425{courses.find(
			make_unique<Course>("MATH", short{425}, 201407))->get()};
	
	Student& student1(*lower_bound(
			students.begin(), students.end(), Student{312995}));
	Student& student2(*lower_bound(
			students.begin(), students.end(), Student{500928}));
	Student& student3(*lower_bound(
			students.begin(), students.end(), Student{147195}));
	Student& student4(*lower_bound(
			students.begin(), students.end(), Student{352468}));
	Student& student5(*lower_bound(
			students.begin(), students.end(), Student{567890}));

	EXPECT_TRUE(student1.HasTakenCourse(english125));
	EXPECT_TRUE(student1.HasTakenCourse(chem210));
	EXPECT_TRUE(student1.HasTakenCourse(chem211));
	EXPECT_TRUE(student1.HasTakenCourse(aaptis277));
	EXPECT_FALSE(student1.HasTakenCourse(environ311));
	EXPECT_FALSE(student1.HasTakenCourse(math425));

	EXPECT_TRUE(student2.HasTakenCourse(english125));
	EXPECT_TRUE(student2.HasTakenCourse(chem210));
	EXPECT_FALSE(student2.HasTakenCourse(chem211));
	EXPECT_FALSE(student2.HasTakenCourse(aaptis277));
	EXPECT_FALSE(student2.HasTakenCourse(environ311));
	EXPECT_FALSE(student2.HasTakenCourse(math425));

	EXPECT_TRUE(student3.HasTakenCourse(english125));
	EXPECT_FALSE(student3.HasTakenCourse(chem210));
	EXPECT_FALSE(student3.HasTakenCourse(chem211));
	EXPECT_TRUE(student3.HasTakenCourse(aaptis277));
	EXPECT_TRUE(student3.HasTakenCourse(environ311));
	EXPECT_FALSE(student3.HasTakenCourse(math425));

	EXPECT_TRUE(student4.HasTakenCourse(english125));
	EXPECT_FALSE(student4.HasTakenCourse(chem210));
	EXPECT_FALSE(student4.HasTakenCourse(chem211));
	EXPECT_FALSE(student4.HasTakenCourse(aaptis277));
	EXPECT_TRUE(student4.HasTakenCourse(environ311));
	EXPECT_TRUE(student4.HasTakenCourse(math425));

	EXPECT_FALSE(student5.HasTakenCourse(english125));
	EXPECT_FALSE(student5.HasTakenCourse(chem210));
	EXPECT_FALSE(student5.HasTakenCourse(chem211));
	EXPECT_FALSE(student5.HasTakenCourse(aaptis277));
	EXPECT_FALSE(student5.HasTakenCourse(environ311));
	EXPECT_TRUE(student5.HasTakenCourse(math425));

	EXPECT_TRUE(english125->IsStudentEnrolled(&student1));
	EXPECT_TRUE(english125->IsStudentEnrolled(&student2));
	EXPECT_TRUE(english125->IsStudentEnrolled(&student3));
	EXPECT_TRUE(english125->IsStudentEnrolled(&student4));
	EXPECT_FALSE(english125->IsStudentEnrolled(&student5));

	EXPECT_TRUE(chem210->IsStudentEnrolled(&student1));
	EXPECT_TRUE(chem210->IsStudentEnrolled(&student2));
	EXPECT_FALSE(chem210->IsStudentEnrolled(&student3));
	EXPECT_FALSE(chem210->IsStudentEnrolled(&student4));
	EXPECT_FALSE(chem210->IsStudentEnrolled(&student5));

	EXPECT_TRUE(chem211->IsStudentEnrolled(&student1));
	EXPECT_FALSE(chem211->IsStudentEnrolled(&student2));
	EXPECT_FALSE(chem211->IsStudentEnrolled(&student3));
	EXPECT_FALSE(chem211->IsStudentEnrolled(&student4));
	EXPECT_FALSE(chem211->IsStudentEnrolled(&student5));

	EXPECT_TRUE(aaptis277->IsStudentEnrolled(&student1));
	EXPECT_FALSE(aaptis277->IsStudentEnrolled(&student2));
	EXPECT_TRUE(aaptis277->IsStudentEnrolled(&student3));
	EXPECT_FALSE(aaptis277->IsStudentEnrolled(&student4));
	EXPECT_FALSE(aaptis277->IsStudentEnrolled(&student5));

	EXPECT_FALSE(environ311->IsStudentEnrolled(&student1));
	EXPECT_FALSE(environ311->IsStudentEnrolled(&student2));
	EXPECT_TRUE(environ311->IsStudentEnrolled(&student3));
	EXPECT_TRUE(environ311->IsStudentEnrolled(&student4));
	EXPECT_FALSE(environ311->IsStudentEnrolled(&student5));

	EXPECT_FALSE(math425->IsStudentEnrolled(&student1));
	EXPECT_FALSE(math425->IsStudentEnrolled(&student2));
	EXPECT_FALSE(math425->IsStudentEnrolled(&student3));
	EXPECT_TRUE(math425->IsStudentEnrolled(&student4));
	EXPECT_TRUE(math425->IsStudentEnrolled(&student5));
}
