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


Course::Id::Id() : subject{Course::undefined_subject}, 
	number{Course::undefined_number}, term{Course::undefined_term} {}


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


ostream& operator<<(ostream& output, const Course::Id& course_id) {
	output << course_id.subject << " " << course_id.number << " "
		   << course_id.term;
	return output;
}

ostream& operator<<(ostream& output, const Course& course) {
	output << course.subject() << " " << course.number() << " " << course.term();
	assert(course.GetNumStudentsEnrolled() >= 0);	
	if (course.GetNumStudentsEnrolled() == 0) { return output; }
	output << " ";
	copy(begin(course.students_enrolled()), 
			--end(course.students_enrolled()),
			ostream_iterator<Student::Id>{output, ", "});
	output << *course.students_enrolled().rbegin();
	return output;
}


istream& operator>>(istream& input, Course& course) {
	// declare variables
	string subject, course_code;
	int term;
	short course_number;
	/* for now, read these into strings
	double grade, gpa_other, cumulative_gpa, total_credits, total_grade_points; 
	*/
	double course_credit;

	// try and read the data, skip the data we're not interested in
	input >> subject >> course_number;
	/*
	input >> course_code >> grade >> gpa_other 
		  >> cumulative_gpa >> total_credits >> total_grade_points;
	*/
	// ignore the fields we don't want for now
	SkipTabField(input);
	SkipTabField(input);
	SkipTabField(input);
	SkipTabField(input);
	SkipTabField(input);
	SkipTabField(input);
	SkipTabField(input);

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
