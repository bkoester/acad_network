#ifndef TAB_READER_H
#define TAB_READER_H

#include <iosfwd>
#include <memory>
#include <vector>
#include <set>

#include "course.hpp"
#include "student.hpp"


struct Enrollment {
	StudentId student_id;
	Course course;
};

std::istream& operator>>(std::istream& input, Enrollment& enrollment);


struct CourseComparator;

using course_container_t = std::set<std::unique_ptr<Course>, CourseComparator>;
using student_container_t = std::vector<Student>;

// Read students into a sorted vector. This vector "owns" the students, other 
// containers must point to them (the vector never grows, so the pointers won't
// be invalidated) or find them in logn with StudentId.
student_container_t ReadStudents(std::istream& student_stream);

// Read StudentIds and courses they took from enrollment data. Populate a set of
// courses based off the courses students took and make a populate the list of
// courses a student took.
course_container_t ReadEnrollment(std::istream& enrollment_stream,
								  student_container_t& students);

// Finds a student with the given ID in the container of students
const Student& FindStudent(StudentId id, const student_container_t& students);
Student& FindStudent(StudentId id, student_container_t& students);

struct CourseComparator {
	bool operator()(const std::unique_ptr<Course>& course1, 
					const std::unique_ptr<Course>& course2) const {
		return *course1 < *course2;
	}
};


#endif  // TAB_READER_H
