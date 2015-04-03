#ifndef COURSE_CONTAINER_H
#define COURSE_CONTAINER_H

#include <memory>
#include <set>
#include <string>

#include "course.hpp"


// incomplete declaration
class StudentContainer;


class CourseContainer {
 public:
	CourseContainer() {}
	// Read Student::Ids and courses they took from enrollment data. Populate a
	// set of courses based off the courses students took and make a populate
	// the list of courses a student took.
	CourseContainer(std::istream& enrollment_stream, StudentContainer& students);

	std::set<std::unique_ptr<Course>, CourseComparator>::size_type 
		size() { return courses_.size(); }

	const std::unique_ptr<Course>& Find(Course course);


 private:
	std::set<std::unique_ptr<Course>, CourseComparator> courses_;
};


class CourseNotFound : public std::exception {
 public:
	CourseNotFound(const Course& course) : 
		error_message_{"Course " + course.subject() + " " 
			+ std::to_string(course.number()) + " " 
			+ std::to_string(course.term()) + " not found!"} {}
	const char* what() const noexcept { return error_message_.c_str(); }

 private:
	std::string error_message_;
};


#endif  // COURSE_CONTAINER_H
