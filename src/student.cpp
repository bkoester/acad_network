#include "student.hpp"

#include <cassert>

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>

#include "course.hpp"
#include "utility.hpp"


using std::bind; using std::placeholders::_1;
using std::begin; using std::end;
using std::copy; using std::transform;
using std::ostream; using std::istream; using std::endl;
using std::ostream_iterator; using std::inserter;


void Student::AddCoursesTaken(std::initializer_list<const Course*> courses) { 
	copy(begin(courses), end(courses), 
			inserter(courses_taken_, end(courses_taken_)));
}

double Student::GetTotalCreditsTaken() const {
	return accumulate(begin(courses_taken_), end(courses_taken_), 0.,
			[](double credits, const Course* course)
			{ return credits + course->num_credits(); });
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
	Student::Ethnicity ethnicity{Student::Ethnicity::Unknown};
	int first_term, degree_term;
	char transfer;

	input >> id >> gender >> ethnicity >> first_term >> degree_term >> transfer;
	if (!input) { return input; }
	SkipLine(input);

	// assign the data to the student
	student.id_ = id;
	student.gender_ = gender;
	student.ethnicity_ = ethnicity;
	student.first_term_ = first_term;
	student.degree_term_ = degree_term;
	student.transfer_ = transfer == 'Y' ? true : false;

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


istream& operator>>(istream& input, Student::Ethnicity& ethnicity) {
	short ethnicity_code;
	input >> ethnicity_code;
	if (!input) { 
		ethnicity = Student::Ethnicity::Unknown;
		return input; 
	}

	// The textual representations are IPEDS conventions.
	switch (ethnicity_code) {
		case 1:
			ethnicity = Student::Ethnicity::Hispanic_only;
			break;
		case 2:
			ethnicity = Student::Ethnicity::American_Indian;
			break;
		case 3:
			ethnicity = Student::Ethnicity::Asian;
			break;
		case 4:
			ethnicity = Student::Ethnicity::Black;
			break;
		case 5:
			ethnicity = Student::Ethnicity::Pacific_Islander;
			break;
		case 6:
			ethnicity = Student::Ethnicity::White;
			break;
		case 7:
			ethnicity = Student::Ethnicity::Multiracial;
			break;
		case 8:
		case 0: // Not sure what this is supposed to be.
			ethnicity = Student::Ethnicity::Unknown;
			break;
		case 9:
			ethnicity = Student::Ethnicity::Undocumented;
			break;
		default:
			input.setstate(std::ios::failbit);
			break;
	}

	return input;
}


ostream& operator<<(ostream& output, const Student::Ethnicity& ethnicity) {
	// The textual representations are IPEDS conventions.
	switch (ethnicity) {
		case Student::Ethnicity::Hispanic_only:
			output << "Hispanic only";
			break;
		case Student::Ethnicity::American_Indian:
			output << "American Indian or Alaska Native";
			break;
		case Student::Ethnicity::Asian:
			output << "Asian only";
			break;
		case Student::Ethnicity::Black:
			output << "Black or African American";
			break;
		case Student::Ethnicity::Pacific_Islander:
			output << "Hawaiian or other Pacific Islander";
			break;
		case Student::Ethnicity::White:
			output << "White only";
			break;
		case Student::Ethnicity::Multiracial:
			output << "Two or more races";
			break;
		case Student::Ethnicity::Undocumented:
			output << "Nonresident alien";
			break;
		default:  // also for "Unknown"
			output << "Unknown";
			break;
	}

	return output;
}


const int Student::uninitialized_id{-1};
const int Student::uninitialized_first_term{-1};
const Student::Ethnicity Student::uninitialized_ethnicity{
	Student::Ethnicity::Unknown};
