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
								 const StudentContainer& students) {

	// Skip the headings line.
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);

		// Insert the course and find the student.
		auto inserted_course = (*courses_.insert(
				make_unique<Course>(enrollment.course)).first).get();
		try {
			// Add the student to the course if the student actually exists (not
			// guaranteed to based on presence in enrollment data).
			const Student& student(students.Find(enrollment.student_id));
			inserted_course->AddStudentEnrolled(&student);
		} catch (StudentNotFound&) {}
	}
}


const unique_ptr<Course>& CourseContainer::Find(Course course) const {
	auto course_ptr = make_unique<Course>(course);
	auto find_it = courses_.find(course_ptr);
	if (std::end(courses_) == find_it) { throw CourseNotFound{course}; }
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
