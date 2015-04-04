#include "student_container.hpp"

#include <cassert>

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>

#include "course_container.hpp"
#include "student.hpp"
#include "utility.hpp"


using std::back_inserter; using std::istream_iterator;
using std::begin; using std::end;
using std::bind; using std::placeholders::_1;
using std::copy; using std::for_each; using std::lower_bound;
using std::initializer_list;
using std::istream;
using std::vector;


StudentContainer::StudentContainer(istream& student_stream) {
	SkipLine(student_stream);
	// copy the students read from the file into the vector
	copy(istream_iterator<Student>{student_stream},
			  istream_iterator<Student>{}, back_inserter(students_));
}


StudentContainer::container_t::iterator StudentContainer::Insert(
		Student student) {
	auto student_it = lower_bound(
			std::begin(students_), std::end(students_), student);
	return students_.insert(student_it, student);
}

void StudentContainer::Insert(
		initializer_list<Student> students) {
	for_each(std::begin(students),  std::end(students), 
			[this](Student student) { Insert(student); });
}


void StudentContainer::UpdateCourses(const CourseContainer& courses) {
	for (const auto& course : courses) {
		for (const auto& student_id : course.students_enrolled()) {
			try {
				// find it again because student_ptr is const
				Student& student(Find(student_id));
				student.AddCourseTaken(&course);
			// CourseContainer should check that all students exist before
			// inserting them, so this should be impossible.
			} catch (StudentNotFound&) { assert(false); }
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


/*
void StudentContainer::Save(std::ostream& output_archive) {
	boost::archive::text_oarchive archive{output_graph_archive};
}

void StudentContainer::Load(std::istream& input_archive) {
	boost::archive::text_iarchive archive{input_graph_archive};
} */
