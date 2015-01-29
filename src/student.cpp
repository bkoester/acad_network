#include "student.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

#include "course.hpp"
#include "utility.hpp"


using std::ostream; using std::istream; using std::endl;
using std::ostream_iterator;
using std::copy;


ostream& operator<<(ostream& os, const Student& student) {
	if (student.id() == Student::uninitialized_id) { os << "ID: Undefined"; }
	else { os << "ID: " << student.id(); }
	os << "Classes: ";
	copy(student.courses_taken_.cbegin(), --student.courses_taken_.cend(), 
			  ostream_iterator<Course>{os, ", "});
	os << *student.courses_taken_.rbegin();
	os << endl;
	return os;
}


istream& operator>>(istream& input, Student& student) {
	// read the input data we're interested in, skip the rest of the line
	StudentId id;
	input >> id;
	if (!input) { return input; }
	SkipLine(input);

	// assign the data to the student
	student.id_ = id;

	return input;
}


const int Student::uninitialized_id{-1};
