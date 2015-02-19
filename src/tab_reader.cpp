#include "tab_reader.hpp"

#include <cassert>

#include <algorithm>
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

		// Insert the course and find the student.
		auto inserted_course = (*courses.insert(
				make_unique<Course>(enrollment.course)).first).get();
		Student& student(FindStudent(enrollment.student_id, students));

		// Add the student to the course.
		inserted_course->AddStudentEnrolled(&student);
		student.AddCourseTaken(inserted_course);
	}

	return courses;
}


const Student& FindStudent(StudentId id, const student_container_t& students) {
	auto student_it = lower_bound(begin(students), end(students), Student{id});
	assert(student_it != end(students));
	return *student_it;
}


Student& FindStudent(StudentId id, student_container_t& students) {
	auto student_it = lower_bound(begin(students), end(students), Student{id});
	assert(student_it != end(students));
	return *student_it;
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
