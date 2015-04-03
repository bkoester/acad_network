#include "course_container.hpp"

#include <iostream>
#include <iterator>
#include <memory>

#include "course.hpp"
#include "student.hpp"
#include "student_container.hpp"
#include "utility.hpp"


using std::istream;
using std::istream_iterator;
using std::unique_ptr;


struct Enrollment {
	Student::Id student_id;
	Course course;
};


static istream& operator>>(istream& input, Enrollment& enrollment);


CourseContainer::CourseContainer(istream& enrollment_stream, 
								 StudentContainer& students) {

	// Skip the headings line.
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);

		// Insert the course and find the student.
		auto inserted_course = (*courses_.insert(
				make_unique<Course>(enrollment.course)).first).get();
		try {
			Student& student(students.Find(enrollment.student_id));

			// Add the student to the course.
			inserted_course->AddStudentEnrolled(&student);
			student.AddCourseTaken(inserted_course);
		} catch (StudentNotFound&) {}
	}
}


const unique_ptr<Course>& CourseContainer::Find(Course course) {
	auto course_ptr = make_unique<Course>(course);
	auto find_it = courses_.find(course_ptr);
	if (end(courses_) == find_it) { throw CourseNotFound{course}; }
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
