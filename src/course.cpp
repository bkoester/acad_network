#include "course.hpp"

#include <iostream>
#include <string>

using std::ostream;
using std::string;

const string undefined{"Undefined"};

Course::Course() : subject_{undefined}, number_{0} {}

ostream& operator<<(ostream& output, const Course& course) {
	output << course.subject() << " " << course.number();
	return output;
}

bool Course::operator<(const Course& other) const {
	// "Undefined" subjects are always at the beginning
	if (subject() == undefined) { return true; }
	if (other.subject() == undefined) { return false; }

	// order by subject name, then number
	if (subject() < other.subject()) { return true; }
	if (subject() > other.subject()) { return false; }
	return number() < other.number();
}
