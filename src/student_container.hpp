#ifndef STUDENT_CONTAINER_H
#define STUDENT_CONTAINER_H

#include <initializer_list>
#include <iosfwd>
#include <iterator>
#include <string>
#include <vector>

#include "student.hpp"


class StudentContainer {
 public:
	StudentContainer() {}

	// Read students into a sorted vector. This vector "owns" the students,
	// other containers must point to them (the vector never grows, so the
	// pointers won't be invalidated) or find them in logn with Student::Id.
	explicit StudentContainer(std::istream& student_stream);

	// Inserts and takes ownership of student.
	virtual std::vector<Student>::iterator Insert(Student student);
	virtual void Insert(std::initializer_list<Student> students);

	virtual ~StudentContainer() {}

	// These functions made virtual for mocking
	// Finds a student with the given ID in the container of students
	virtual Student& Find(Student::Id id);
	virtual const Student& Find(Student::Id id) const;

	virtual std::vector<Student>::size_type size() const 
	{ return students_.size(); }

	virtual std::vector<Student>::iterator begin() 
	{ return std::begin(students_); }
	virtual std::vector<Student>::const_iterator begin() const
	{ return std::begin(students_); }
	virtual std::vector<Student>::const_iterator cbegin() const
	{ return students_.cbegin(); }

	virtual std::vector<Student>::iterator end() 
	{ return std::end(students_); }
	virtual std::vector<Student>::const_iterator end() const
	{ return std::end(students_); }
	virtual std::vector<Student>::const_iterator cend() const
	{ return students_.cend(); }

 private:
	std::vector<Student> students_;
};


class StudentNotFound : public std::exception {
 public:
	StudentNotFound(Student::Id id) : 
		error_message_{"Student " + std::to_string(id) + " not found!"} {}
	const char* what() const noexcept { return error_message_.c_str(); }

 private:
	std::string error_message_;
};


#endif  // STUDENT_CONTAINER_H
