#include "tab_reader.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"


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

	Course english125{*courses.find(Course{"ENGLISH", 125})};
	Course chem210{*courses.find(Course{"CHEM", 210})};
	Course chem211{*courses.find(Course{"CHEM", 211})};
	Course aaptis277{*courses.find(Course{"AAPTIS", 277})};
	Course environ311{*courses.find(Course{"ENVIRON", 311})};
	Course math425{*courses.find(Course{"MATH", 425})};

	EXPECT_EQ(1u, courses.count(english125));
	EXPECT_EQ(1u, courses.count(chem210));
	EXPECT_EQ(1u, courses.count(chem211));
	EXPECT_EQ(1u, courses.count(aaptis277));
	EXPECT_EQ(1u, courses.count(environ311));
	EXPECT_EQ(1u, courses.count(math425));

	Student student1{*lower_bound(
			students.begin(), students.end(), Student{312995})};
	Student student2{*lower_bound(
			students.begin(), students.end(), Student{500928})};
	Student student3{*lower_bound(
			students.begin(), students.end(), Student{147195})};
	Student student4{*lower_bound(
			students.begin(), students.end(), Student{352468})};
	Student student5{*lower_bound(
			students.begin(), students.end(), Student{567890})};

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

}
