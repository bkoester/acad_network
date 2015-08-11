#ifndef STUDENT_CONTAINER_H
#define STUDENT_CONTAINER_H

#include <exception>
#include <initializer_list>
#include <iosfwd>
#include <iterator>
#include <string>
#include <vector>

#include <boost/serialization/vector.hpp>

#include "student.hpp"


// incomplete declaration
class CourseContainer;


class StudentContainer {
 public:
	using container_t = std::vector<Student>;

	// Read students into a sorted vector. This vector "owns" the students,
	// other containers must point to them (the vector never grows, so the
	// pointers won't be invalidated) or find them in logn with Student::Id.
	static StudentContainer LoadFromTsv(std::istream& student_stream);
	// Load the student container from a Boost archive.
	static StudentContainer LoadFromArchive(std::istream& input_archive);
	static StudentContainer LoadFromArchive(std::string input_path);
    // Save the student container to a Boost archive.
	void SaveToArchive(std::ostream& output);

	virtual ~StudentContainer() {}

	bool operator==(const StudentContainer& other) const
	{ return students_ == other.students_; }

	// Inserts and takes ownership of student.
	container_t::iterator Insert(Student student);
	void Insert(std::initializer_list<Student> students);

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int) { ar & students_; }

	// Populate the list of courses a student took.
	void UpdateCourses(const CourseContainer& courses);

	// These functions made virtual for mocking
	// Finds a student with the given ID in the container of students
	virtual Student& Find(Student::Id id);
	virtual const Student& Find(Student::Id id) const;

	virtual container_t::size_type size() const
	{ return students_.size(); }

	container_t::iterator begin() { return std::begin(students_); }
	container_t::const_iterator begin() const { return std::begin(students_); }
	container_t::const_iterator cbegin() const { return students_.cbegin(); }

	container_t::iterator end() { return std::end(students_); }
	container_t::const_iterator end() const { return std::end(students_); }
	container_t::const_iterator cend() const { return students_.cend(); }

 protected:
	StudentContainer() {}  // use "Load..." static member functions instead

 private:
    friend class StudentContainerTest;  // to access default constructor

	container_t students_;
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
