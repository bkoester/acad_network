#include "student_container.hpp"

#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"
#include "utility.hpp"


using std::string;
using std::stringstream;

TEST(StudentContainerTest, Find) {
	stringstream student_stream{student_tab};
	auto students = StudentContainer{student_stream};
	auto unused_id1 = 0;
	auto unused_id2 = 300000;
	auto unused_id3 = 999999;

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
