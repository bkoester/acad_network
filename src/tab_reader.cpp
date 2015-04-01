#include "tab_reader.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

#include "course.hpp"
#include "student.hpp"
#include "utility.hpp"


using std::back_inserter; using std::istream_iterator; 
using std::copy; using std::lower_bound;
using std::istream;


Student::container_t ReadStudents(istream& student_stream) {
	Student::container_t students;
	SkipLine(student_stream);
	// copy the students read from the file into the vector
	copy(istream_iterator<Student>{student_stream},
			  istream_iterator<Student>{}, back_inserter(students));
	
	return students;
}


Course::container_t ReadEnrollment(istream& enrollment_stream, 
								  Student::container_t& students) {
	Course::container_t courses;

	// Skip the headings line.
	SkipLine(enrollment_stream);
	istream_iterator<Enrollment> enrollment_it{enrollment_stream};

	for (;enrollment_it != istream_iterator<Enrollment>{}; ++enrollment_it) {
		const Enrollment& enrollment(*enrollment_it);

		// Insert the course and find the student.
		auto inserted_course = (*courses.insert(
				make_unique<Course>(enrollment.course)).first).get();
		try {
			Student& student(FindStudent(enrollment.student_id, students));

			// Add the student to the course.
			inserted_course->AddStudentEnrolled(&student);
			student.AddCourseTaken(inserted_course);
		} catch (StudentNotFound&) {}
	}

	return courses;
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
