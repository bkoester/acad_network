#include "course.hpp"

#include <cassert>

#include <algorithm>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>

#include "student.hpp"
#include "utility.hpp"


using std::begin; using std::end; using std::ostream_iterator;
using std::istream; using std::ostream; using std::endl;
using std::numeric_limits;
using std::streamsize;
using std::string;
using std::transform;


bool Course::operator<(const Course& other) const {
	// "Undefined" subjects are always at the beginning
	if (subject() == undefined_subject) { return true; }
	if (other.subject() == undefined_subject) { return false; }

	// order by subject name, then number, then term
	if (subject() < other.subject()) { return true; }
	if (subject() > other.subject()) { return false; }
	if (number() < other.number()) { return true; }
	if (number() > other.number()) { return false; }
	return term() < other.term();
}


ostream& operator<<(ostream& output, const Course& course) {
	output << course.subject() << " " << course.number() << " " << course.term();
	assert(course.GetNumStudentsEnrolled() >= 0);	
	if (course.GetNumStudentsEnrolled() == 0) { return output; }
	output << " ";
	transform(begin(course.students_enrolled()), 
			--end(course.students_enrolled()),
			ostream_iterator<StudentId>{output, ", "},
			[](const Student* student) { return student->id(); });
	output << (*course.students_enrolled().rbegin())->id();
	return output;
}


istream& operator>>(istream& input, Course& course) {
	// declare variables
	string subject, course_code;
	int term;
	short course_number;
	double grade, gpa_other, cumulative_gpa, total_credits, total_grade_points,
			course_credit;

	// try and read the data, skip the data we're not interested in
	input >> subject >> course_number;
	input >> course_code >> grade >> gpa_other 
		  >> cumulative_gpa >> total_credits >> total_grade_points;
	assert(!course_code.empty() || course_code.empty());
	assert(grade != 0.0 || grade == 0.0); 
	assert(gpa_other != 0.0 || gpa_other == 0.0); 
	assert(cumulative_gpa != 0.0 || cumulative_gpa == 0.0);
	assert(total_grade_points != 0.0 || total_grade_points == 0.0); 
	assert(total_credits != 0.0 || total_credits == 0.0); 
	// ignore the fields we don't want for now
	/*
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t');
	input.ignore(numeric_limits<streamsize>::max(), '\t'); */
	input >> course_credit >> term;
	if (!input) { return input; }
	SkipLine(input);

	// update the course based on the information gained from the line
	course.subject_ = subject;
	course.number_ = course_number;
	course.term_ = term;
	course.num_credits_ = course_credit;
	
	return input;
}


const string Course::undefined_subject{"undefined"};
const short Course::undefined_number{0};
const int Course::undefined_term{0};
