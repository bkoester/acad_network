#include "student.hpp"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iterator>

#include "course.hpp"
#include "utility.hpp"


using std::begin; using std::end;
using std::copy; using std::transform;
using std::ostream; using std::istream; using std::endl;
using std::ostream_iterator; using std::inserter;


void Student::AddCoursesTaken(std::initializer_list<const Course*> courses) { 
	copy(begin(courses), end(courses), 
			inserter(courses_taken_, end(courses_taken_)));
}


ostream& operator<<(ostream& os, const Student& student) {
	// print ID
	os << "ID: ";
	if (student.id() == Student::uninitialized_id) { os << "Undefined"; }
	else { os << student.id(); }

	os << "; Gender: " << student.gender();
	os << "; Ethnicity: " << student.ethnicity();

	// print first term
	os << "; First term: ";
	if (student.first_term() == Student::uninitialized_first_term)
	{ os << "Undefined"; }
	else { os << student.first_term(); }
	os << "; Classes: ";
	transform(student.courses_taken_.cbegin(), --student.courses_taken_.cend(), 
			  ostream_iterator<CourseId>{os, ", "}, 
			  [](const Course* course) { return course->GetId(); });
	os << *student.courses_taken_.rbegin();
	os << endl;
	return os;
}


istream& operator>>(istream& input, Student& student) {
	// read the input data we're interested in, skip the rest of the line
	StudentId id;
	Student::Gender gender;
	Student::Ethnicity ethnicity;
	int first_term;

	input >> id >> gender >> ethnicity >> first_term;
	if (!input) { return input; }
	SkipLine(input);

	// assign the data to the student
	student.id_ = id;
	student.gender_ = gender;
	student.ethnicity_ = ethnicity;
	student.first_term_ = first_term;

	return input;
}


istream& operator>>(istream& input, Student::Gender& gender) {
	char c;
	input >> c;

	// assign gender based on input
	switch (c) {
		case 'M':
			gender = Student::Gender::Male;
			break;
		case 'F':
			gender = Student::Gender::Female;
			break;
		default:
			gender = Student::Gender::Unspecified; 
			break;
	}

	return input;
}


ostream& operator<<(ostream& output, const Student::Gender& gender) {
	switch (gender) {
		case Student::Gender::Male:
			output << "Male";
			break;
		case Student::Gender::Female:
			output << "Female";
			break;
		default:
			output << "Unspecified";
			break;
	}

	return output;
}


const int Student::uninitialized_id{-1};
const int Student::uninitialized_first_term{-1};
const Student::Ethnicity Student::uninitialized_ethnicity{-1};
