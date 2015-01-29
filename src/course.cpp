#include "course.hpp"

#include <iostream>
#include <string>

#include "utility.hpp"


using std::ostream; using std::istream;
using std::string;


bool Course::operator<(const Course& other) const {
	// "Undefined" subjects are always at the beginning
	if (subject() == undefined_subject) { return true; }
	if (other.subject() == undefined_subject) { return false; }

	// order by subject name, then number
	if (subject() < other.subject()) { return true; }
	if (subject() > other.subject()) { return false; }
	return number() < other.number();
}


ostream& operator<<(ostream& output, const Course& course) {
	output << course.subject() << " " << course.number();
	return output;
}


istream& operator>>(istream& input, Course& course) {
	// declare variables
	string subject;
	int course_number;

	// try and read the data, skip the data we're not interested in
	input >> subject >> course_number;
	if (!input) { return input; }
	SkipLine(input);

	// store line state
	course.subject_ = subject;
	course.number_ = course_number;
	
	return input;
}


const string Course::undefined_subject{"undefined"};
const int Course::undefined_number{0};
