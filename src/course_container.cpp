#include "course_container.hpp"

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "course.hpp"
#include "student.hpp"
#include "student_container.hpp"
#include "utility.hpp"


using std::for_each;
using std::initializer_list;
using std::istream; using std::ostream;
using std::istream_iterator;
using std::unique_ptr;


struct Enrollment {
	Student::Id student_id;
	Course course;
};


static istream& operator>>(istream& input, Enrollment& enrollment);


CourseContainer::CourseContainer(istream& enrollment_stream, 
								 const StudentContainer& students) {

	// Skip the headings line.
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);

		// Insert the course and find the student.
		auto inserted_course = &(*Insert(enrollment.course));

		try {
			// Add the student to the course if the student actually exists (not
			// guaranteed to based on presence in enrollment data).
			const Student& student(students.Find(enrollment.student_id));
			inserted_course->AddStudentEnrolled(student.id());
		} catch (StudentNotFound&) {}
	}
}


CourseContainer::container_t::iterator CourseContainer::Insert(
		Course course) {
	auto course_it = lower_bound(
			std::begin(courses_), std::end(courses_), course);
	if (course_it != std::end(courses_) && *course_it == course)
	{ return course_it; }
	return courses_.insert(course_it, course);
}


void CourseContainer::Insert(
		initializer_list<Course> courses) {
	for_each(std::begin(courses), std::end(courses), 
			[this](Course course) { Insert(course); });
}


const Course& CourseContainer::Find(Course course) const {
	auto find_it = lower_bound(
			std::begin(courses_), std::end(courses_), course);
	if (std::end(courses_) == find_it || *find_it != course)
	{ throw CourseNotFound{course}; }
	return *find_it;
}


Course& CourseContainer::Find(Course course) {
	auto find_it = lower_bound(
			std::begin(courses_), std::end(courses_), course);
	if (std::end(courses_) == find_it || *find_it != course)
	{ throw CourseNotFound{course}; }
	return *find_it;
}

void CourseContainer::Save(ostream& output) {
	boost::archive::text_oarchive archive{output};
	serialize(archive, 0);
}

void CourseContainer::Load(istream& input) {
	boost::archive::text_iarchive archive{input};
	serialize(archive, 0);
}


istream& operator>>(istream& input, Enrollment& enrollment) {
	Student::Id student_id;
	Course course;

	input >> student_id >> course;
	if (!input) { return input; }

	enrollment.student_id = student_id;
	enrollment.course = course;

	return input;
}
