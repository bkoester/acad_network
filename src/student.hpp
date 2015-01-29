#ifndef STUDENT_H
#define STUDENT_H

#include <functional>
#include <iosfwd>
#include <set>

#include <boost/serialization/utility.hpp>


class Course;


using StudentId = int;
using StudentIdHasher = std::hash<int>;


class Student {
 public:
	Student() : id_{uninitialized_id} {}
	Student(StudentId id) : id_{id} {}

	int id() const { return id_; }

	bool operator==(const Student& other) const { return id() == other.id(); }
	bool operator!=(const Student& other) const { return !(operator==(other)); }
	bool operator<(const Student& other) const { return id() < other.id(); }

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) { ar & id_; }

	void AddCourseTaken(const Course& course) { courses_taken_.insert(course); }
	bool HasTakenCourse(const Course& course) const
	{ return courses_taken_.count(course) == 1; }

 private:
	friend class StudentTest;
	friend class boost::serialization::access;
	friend std::ostream& operator<<(std::ostream& os, const Student& student);
	friend std::istream& operator>>(std::istream& input, Student& student);

	StudentId id_;
	std::set<Course> courses_taken_;
	static const int uninitialized_id;
};


#endif  // STUDENT_H
