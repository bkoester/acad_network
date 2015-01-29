#ifndef TAB_READER_H
#define TAB_READER_H

#include <iosfwd>
#include <memory>
#include <vector>
#include <set>

#include "course.hpp"
#include "student.hpp"

using course_container_t = std::set<Course>;
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


#endif  // TAB_READER_H
