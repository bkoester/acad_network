#include "course_container.hpp"

#include <algorithm>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "course.hpp"
#include "student.hpp"
#include "student_container.hpp"
#include "utility.hpp"


using std::for_each;
using std::ifstream;
using std::initializer_list;
using std::istream; using std::ostream;
using std::istream_iterator;
using std::string;
using std::unique_ptr;


struct Enrollment {
	Student::Id student_id;
	Course course;
};


static istream& operator>>(istream& input, Enrollment& enrollment);


CourseContainer CourseContainer::LoadFromTsv(istream& enrollment_stream) {
    CourseContainer course_container{};

	// Skip the headings line.
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);

		// Insert the course to the course container.
		auto inserted_course_it = course_container.Insert(enrollment.course);
        auto inserted_course = &(*inserted_course_it);

        // Insert the student ID even if a corresponding student does not exist
        // in the student container. Not having the check the student container
        // makes the design more intuitive.
        inserted_course->AddStudentEnrolled(enrollment.student_id);
	}
    
    return course_container;
}


CourseContainer CourseContainer::LoadFromArchive(istream& input_archive) {
    CourseContainer course_container{};

	boost::archive::text_iarchive archive{input_archive};
	course_container.serialize(archive, 0);

    return course_container;
}


CourseContainer CourseContainer::LoadFromArchive(string input_path) {
    ifstream input_archive{input_path};
    return LoadFromArchive(input_archive);
}


void CourseContainer::SaveToArchive(ostream& output) {
	boost::archive::text_oarchive archive{output};
	serialize(archive, 0);
}


CourseContainer::container_t::iterator CourseContainer::Insert(Course course) {
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


istream& operator>>(istream& input, Enrollment& enrollment) {
	Student::Id student_id;
	Course course;

	input >> student_id >> course;
	if (!input) { return input; }

	enrollment.student_id = student_id;
	enrollment.course = course;

	return input;
}
