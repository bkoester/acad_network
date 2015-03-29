#ifndef TAB_READER_H
#define TAB_READER_H

#include <iosfwd>

#include "course.hpp"
#include "student.hpp"


struct Enrollment {
	Student::Id student_id;
	Course course;
};

std::istream& operator>>(std::istream& input, Enrollment& enrollment);


// Read students into a sorted vector. This vector "owns" the students, other 
// containers must point to them (the vector never grows, so the pointers won't
// be invalidated) or find them in logn with Student::Id.
Student::container_t ReadStudents(std::istream& student_stream);

// Read Student::Ids and courses they took from enrollment data. Populate a set of
// courses based off the courses students took and make a populate the list of
// courses a student took.
Course::container_t ReadEnrollment(std::istream& enrollment_stream,
								  Student::container_t& students);


#endif  // TAB_READER_H
