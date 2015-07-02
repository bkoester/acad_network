#include "student_container.hpp"

#include <cassert>

#include <algorithm>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "course_container.hpp"
#include "student.hpp"
#include "utility.hpp"


using std::back_inserter; using std::istream_iterator;
using std::begin; using std::end;
using std::bind; using std::placeholders::_1;
using std::copy; using std::for_each; using std::lower_bound;
using std::ifstream;
using std::initializer_list;
using std::istream; using std::ostream;
using std::string;
using std::vector;


StudentContainer StudentContainer::LoadFromTsv(istream& student_stream) {
    StudentContainer students{};

    // Skip headings line.
	SkipLine(student_stream);
	// Copy the students read from the file into the vector.
	copy(istream_iterator<Student>{student_stream},
			  istream_iterator<Student>{}, back_inserter(students.students_));

    return students;
}


StudentContainer StudentContainer::LoadFromArchive(istream& input_archive) {
    StudentContainer students{};

	boost::archive::text_iarchive archive{input_archive};
	students.serialize(archive, 0);

    return students;
}


StudentContainer StudentContainer::LoadFromArchive(string input_path) {
    ifstream input_archive{input_path};
    return LoadFromArchive(input_archive);
}


void StudentContainer::SaveToArchive(ostream& output_archive) {
	boost::archive::text_oarchive archive{output_archive};
	serialize(archive, 0);
}


StudentContainer::container_t::iterator StudentContainer::Insert(
		Student student) {
	auto student_it = lower_bound(
			std::begin(students_), std::end(students_), student);
	return students_.insert(student_it, student);
}


void StudentContainer::Insert(
		initializer_list<Student> students) {
	for_each(std::begin(students), std::end(students), 
			[this](Student student) { Insert(student); });
}


void StudentContainer::UpdateCourses(const CourseContainer& courses) {
	for (const auto& course : courses) {
		for (const auto& student_id : course.students_enrolled()) {
			try {
				// find student corresponding to ID
				Student& student(Find(student_id));
				student.AddCourseTaken(&course);
			// There may be courses that have Student IDs that don't exist in
            // the student container, ignore them.
			} catch (StudentNotFound&) {}
		}
	}
}


const Student& StudentContainer::Find(Student::Id id) const {
	auto student_it = lower_bound(
			std::begin(students_), std::end(students_), Student{id});
	if (student_it == std::end(students_) || student_it->id() != id)
	{ throw StudentNotFound{id}; }
	return *student_it;
}


Student& StudentContainer::Find(Student::Id id) {
	auto student_it = lower_bound(
			std::begin(students_), std::end(students_), Student{id});
	if (student_it == std::end(students_) || student_it->id() != id)
	{ throw StudentNotFound{id}; }
	return *student_it;
}
