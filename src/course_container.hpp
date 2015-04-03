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
	using container_t = std::set<std::unique_ptr<Course>, CourseComparator>;

	CourseContainer() {}
	// Read Student::Ids and courses they took from enrollment data. 
	// Populate a set of courses and which students took them
	explicit CourseContainer(std::istream& enrollment_stream,
							 const StudentContainer& students);


	std::set<std::unique_ptr<Course>, CourseComparator>::size_type 
		size() { return courses_.size(); }

	const std::unique_ptr<Course>& Find(Course course) const;

	virtual container_t::iterator begin() { return std::begin(courses_); }
	virtual container_t::const_iterator begin() const
	{ return std::begin(courses_); }
	virtual container_t::const_iterator cbegin() const
	{ return courses_.cbegin(); }

	virtual container_t::iterator end() { return std::end(courses_); }
	virtual container_t::const_iterator end() const
	{ return std::end(courses_); }
	virtual container_t::const_iterator cend() const { return courses_.cend(); }


 private:
	container_t courses_;
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
