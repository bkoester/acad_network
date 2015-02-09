#include "tab_reader.hpp"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

#include "utility.hpp"


using std::back_inserter; using std::istream_iterator; 
using std::copy; using std::lower_bound;
using std::istream;
using std::set;


student_container_t ReadStudents(istream& student_stream) {
	student_container_t students;
	SkipLine(student_stream);
	// copy the students read from the file into the vector
	copy(istream_iterator<Student>{student_stream},
			  istream_iterator<Student>{}, back_inserter(students));
	
	return students;
}


course_container_t ReadEnrollment(istream& enrollment_stream, 
								  student_container_t& students) {
	course_container_t courses;

	// Skip the headings line.
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);

		// Insert the course and get a pointer to it.
		auto inserted_course = (*courses.insert(
				make_unique<Course>(enrollment.course)).first).get();
		
		// Find the student.
		auto student_it = lower_bound(begin(students), end(students),
				Student{enrollment.student_id});
		assert(student_it != students.end());
		Student& student{*student_it};

		// Add the student to the course.
		inserted_course->AddStudentEnrolled(&student);
		student.AddCourseTaken(inserted_course);
	}


	return courses;
}


istream& operator>>(istream& input, Enrollment& enrollment) {
	StudentId student_id;
	Course course;

	input >> student_id >> course;
	if (!input) { return input; }

	enrollment.student_id = student_id;
	enrollment.course = course;

	return input;
}
