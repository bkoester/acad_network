#include "student_container.hpp"

#include <iterator>
#include <sstream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "course.hpp"
#include "course_container_mock.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"
#include "utility.hpp"


using std::begin; using std::end;
using std::move;
using std::string;
using std::stringstream;


class StudentContainerTest : public ::testing::Test {
 public:
	void SetUp() override {
		unused_id1 = 0;
		unused_id2 = 300000;
		unused_id3 = 999999;

		stringstream student_stream{student_tab};
		students = StudentContainer{student_stream};
	}
	
	
 protected:
	StudentContainer students;
	Student::Id unused_id1, unused_id2, unused_id3;
};


TEST_F(StudentContainerTest, Find) {
	EXPECT_EQ(5u, students.size());

	EXPECT_EQ(Student{147195}, students.Find(147195));  
	EXPECT_EQ(Student{312995}, students.Find(312995));
	EXPECT_EQ(Student{352468}, students.Find(352468));
	EXPECT_EQ(Student{500928}, students.Find(500928));
	EXPECT_EQ(Student{567890}, students.Find(567890));
	EXPECT_THROW(students.Find(unused_id1), StudentNotFound);
	EXPECT_THROW(students.Find(unused_id2), StudentNotFound);
	EXPECT_THROW(students.Find(unused_id3), StudentNotFound);

	const auto& const_students = StudentContainer{students};
	EXPECT_EQ(Student{147195}, const_students.Find(147195));  
	EXPECT_EQ(Student{312995}, const_students.Find(312995));
	EXPECT_EQ(Student{352468}, const_students.Find(352468));
	EXPECT_EQ(Student{500928}, const_students.Find(500928));
	EXPECT_EQ(Student{567890}, const_students.Find(567890));
	EXPECT_THROW(const_students.Find(unused_id1), StudentNotFound);
	EXPECT_THROW(const_students.Find(unused_id2), StudentNotFound);
	EXPECT_THROW(const_students.Find(unused_id3), StudentNotFound);
}


TEST_F(StudentContainerTest, Insert) {
	students.Insert(
			{Student{unused_id1}, Student{unused_id2}, Student{unused_id3}});

	EXPECT_EQ(Student{unused_id1}, students.Find(unused_id1));  
	EXPECT_EQ(Student{unused_id2}, students.Find(unused_id2));
	EXPECT_EQ(Student{unused_id3}, students.Find(unused_id3));
}


TEST_F(StudentContainerTest, UpdateCourses) {
	EXPECT_EQ(5u, students.size());

	// get the students out of the container
	auto students_it = begin(students);
	const auto& student1 = *students_it++;
	const auto& student2 = *students_it++;
	const auto& student3 = *students_it++;
	const auto& student4 = *students_it++;
	const auto& student5 = *students_it++;

	EXPECT_EQ(end(students), students_it);

	// create courses that students should be enrolled in
	MockCourseContainer::container_t courses_holder;
	courses_holder.insert(move(
				make_unique<Course>("ENGLISH", short{125}, 201403)));
	courses_holder.insert(move(
		make_unique<Course>("CHEM", short{210}, 201403)));
	courses_holder.insert(move(
		make_unique<Course>("CHEM", short{211}, 201407)));
	courses_holder.insert(move(
		make_unique<Course>("AAPTIS", short{277}, 201403)));
	courses_holder.insert(move(
		make_unique<Course>("ENVIRON", short{311}, 201405)));
	courses_holder.insert(move(
		make_unique<Course>("MATH", short{425}, 201407)));

	const auto& course1 = *courses_holder.find(
			make_unique<Course>("ENGLISH", short{125}, 201403));
	course1->AddStudentEnrolled(&student1);
	course1->AddStudentEnrolled(&student2);
	course1->AddStudentEnrolled(&student3);

	const auto& course2 = *courses_holder.find(
		make_unique<Course>("CHEM", short{210}, 201403));
	course2->AddStudentEnrolled(&student1);
	course2->AddStudentEnrolled(&student4);

	const auto& course3 = *courses_holder.find(
		make_unique<Course>("CHEM", short{211}, 201407));
	course3->AddStudentEnrolled(&student3);
	course3->AddStudentEnrolled(&student4);

	const auto& course4 = *courses_holder.find(
		make_unique<Course>("AAPTIS", short{277}, 201403));
	course4->AddStudentEnrolled(&student2);
	course4->AddStudentEnrolled(&student3);
	course4->AddStudentEnrolled(&student5);

	const auto& course5 = *courses_holder.find(
		make_unique<Course>("ENVIRON", short{311}, 201405));
	course5->AddStudentEnrolled(&student1);

	const auto& course6 = *courses_holder.find(
		make_unique<Course>("MATH", short{425}, 201407));

	// create CourseContainer mock and set call expectations
	const MockCourseContainer courses;
	using ::testing::AtLeast;
	using ::testing::Return;
	EXPECT_CALL(courses, begin()).Times(AtLeast(1)).WillRepeatedly(
			Return(begin(courses_holder)));
	EXPECT_CALL(courses, end()).Times(AtLeast(1)).WillRepeatedly(
			Return(end(courses_holder)));

	students.UpdateCourses(courses);

	// check that students were updated
	EXPECT_TRUE(student1.HasTakenCourse(course1.get()));
	EXPECT_TRUE(student2.HasTakenCourse(course1.get()));
	EXPECT_TRUE(student3.HasTakenCourse(course1.get()));
	EXPECT_FALSE(student4.HasTakenCourse(course1.get()));
	EXPECT_FALSE(student5.HasTakenCourse(course1.get()));

	EXPECT_TRUE(student1.HasTakenCourse(course2.get()));
	EXPECT_FALSE(student2.HasTakenCourse(course2.get()));
	EXPECT_FALSE(student3.HasTakenCourse(course2.get()));
	EXPECT_TRUE(student4.HasTakenCourse(course2.get()));
	EXPECT_FALSE(student5.HasTakenCourse(course2.get()));

	EXPECT_FALSE(student1.HasTakenCourse(course3.get()));
	EXPECT_FALSE(student2.HasTakenCourse(course3.get()));
	EXPECT_TRUE(student3.HasTakenCourse(course3.get()));
	EXPECT_TRUE(student4.HasTakenCourse(course3.get()));
	EXPECT_FALSE(student5.HasTakenCourse(course3.get()));

	EXPECT_FALSE(student1.HasTakenCourse(course4.get()));
	EXPECT_TRUE(student2.HasTakenCourse(course4.get()));
	EXPECT_TRUE(student3.HasTakenCourse(course4.get()));
	EXPECT_FALSE(student4.HasTakenCourse(course4.get()));
	EXPECT_TRUE(student5.HasTakenCourse(course4.get()));

	EXPECT_TRUE(student1.HasTakenCourse(course5.get()));
	EXPECT_FALSE(student2.HasTakenCourse(course5.get()));
	EXPECT_FALSE(student3.HasTakenCourse(course5.get()));
	EXPECT_FALSE(student4.HasTakenCourse(course5.get()));
	EXPECT_FALSE(student5.HasTakenCourse(course5.get()));

	EXPECT_FALSE(student1.HasTakenCourse(course6.get()));
	EXPECT_FALSE(student2.HasTakenCourse(course6.get()));
	EXPECT_FALSE(student3.HasTakenCourse(course6.get()));
	EXPECT_FALSE(student4.HasTakenCourse(course6.get()));
	EXPECT_FALSE(student5.HasTakenCourse(course6.get()));
}
