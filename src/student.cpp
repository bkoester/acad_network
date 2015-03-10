#include "student.hpp"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include "course.hpp"
#include "utility.hpp"


using std::begin; using std::end;
using std::copy; using std::transform;
using std::ostream; using std::istream; using std::endl;
using std::ostream_iterator; using std::inserter;
using std::string; using std::stod;
using std::unordered_map;


void Student::AddCoursesTaken(std::initializer_list<const Course*> courses) { 
	copy(begin(courses), end(courses), 
			inserter(courses_taken_, end(courses_taken_)));
}


double Student::GetTotalCreditsTaken() const {
	return accumulate(begin(courses_taken_), end(courses_taken_), 0.,
			[](double credits, const Course* course)
			{ return credits + course->num_credits(); });
}


string Student::GetMajor1Description() const {
	if (!major1_) { return "NA"; }

	double major_code{major1_.get()};
	assert(major_code_map.count(major_code) == 1);
	return major_code_map.at(major_code);
}


string Student::GetMajor2Description() const {
	if (!major2_) { return "NA"; }

	double major_code{major2_.get()};
	assert(major_code_map.count(major_code) == 1);
	return major_code_map.at(major_code);
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
	os << "; Major(s): ";
	if (student.major1()) { os << student.GetMajor1Description(); }
	if (student.major2()) { os << ", " << student.GetMajor2Description(); }
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
	string major1, major2;

	input >> id >> gender >> ethnicity >> first_term >> degree_term >> transfer
		  >> major1 >> major2;
	if (!input) { return input; }

	SkipTabField(input);  // major1 description
	SkipTabField(input);  // major2 description
	SkipTabField(input);  // first declare code
	SkipTabField(input);  // first declare description
	SkipTabField(input);  // first declare term
	SkipTabField(input);  // pell status
	SkipTabField(input);  // ACT english
	SkipTabField(input);  // ACT math
	SkipTabField(input);  // ACT comp

	string school;
	input >> school;

	SkipLine(input);

	// assign the data to the student
	student.id_ = id;
	student.gender_ = gender;
	student.ethnicity_ = ethnicity;
	student.first_term_ = first_term;
	student.degree_term_ = degree_term;
	student.transfer_ = transfer == 'Y' ? true : false;
	if (major1 != "NA") { student.major1_ = stod(major1); }
	if (major2 != "NA") { student.major2_ = stod(major2); }
	student.school_ = school;

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
const unordered_map<double, string> Student::major_code_map{
	{3.0103, "Environmental Studies"},
	{4.0201, "Architecture (BArch, BA/BS, MA"},
	{5.0102, "American/United States Studies"},
	{5.0103, "Asian Studies/Civilization"},
	{5.0108, "Near and Middle Eastern Studies"},
	{5.011, "Russian Studies"},
	{5.0134, "Latin American and Caribbean Studies"},
	{5.0201, "African-American/Black Studies"},
	{5.0203, "Hispanic-American, Puerto Rican Studies"},
	{5.0207, "Women's Studies"},
	{9.0101, "Communication Studies/Speech Communication"},
	{11.0101, "Computer and Information Science"},
	{11.0104, "Informatics."},
	{13.1202, "Elementary Education and Teaching"},
	{13.1205, "Secondary Education and Teaching"},
	{13.1312, "Music Teacher Education"},
	{13.1314, "Physical Education Teaching"},
	{14.0101, "Engineering, General"},
	{14.0201, "Aerospace, Aeronautical"},
	{14.0501, "Biomedical/Medical Engineering"},
	{14.0701, "Chemical Engineering"},
	{14.0801, "Civil Engineering, General"},
	{14.0901, "Computer Engineering, General"},
	{14.1001, "Electrical, Electronics and Computer Engineering"},
	{14.1201, "Engineering Physics"},
	{14.1401, "Environmental"},
	{14.1801, "Materials Engineering"},
	{14.1901, "Mechanical Engineering"},
	{14.2201, "Naval Architecture and Marine"},
	{14.2301, "Nuclear Engineering"},
	{14.3501, "Industrial Engineering"},
	{14.3901, "Geological/Geophysical Enginee"},
	{14.9999, "Engineering, Other"},
	{16.0102, "Linguistics"},
	{16.0104, "Comparative Literature"},
	{16.0402, "Russian Language and Literature"},
	{16.0407, "Polish Language and Literature"},
	{16.0501, "German Language and Literature"},
	{16.0601, "Modern Greek Language and Literature"},
	{16.09, "Romance Languages, Literatures"},
	{16.0901, "French Language and Literature"},
	{16.0902, "Italian Language and Literature"},
	{16.0905, "Spanish Language and Literature"},
	{16.1102, "Hebrew Language and Literature"},
	{16.1199, "Middle/Near Eastern and Semitic"},
	{16.12, "Classics and Languages, Literature"},
	{16.1202, "Ancient/Classical Greek Language"},
	{16.1203, "Latin Language and Literature"},
	{23.0101, "English Language and Literature"},
	{23.1302, "Creative Writing."},
	{24.0102, "General Studies"},
	{24.0103, "Humanities/Humanistic Studies"},
	{26.0101, "Biology/Biological Sciences, General"},
	{26.0202, "Biochemistry"},
	{26.0203, "Biophysics"},
	{26.0204, "Molecular Biology"},
	{26.0406, "Cell/Cellular and Molecular Biology"},
	{26.0502, "Microbiology, General"},
	{26.131, "Ecology and Evolutionary Biology"},
	{26.1501, "Neuroscience."},
	{26.9999, "Biological and Biomedical Sciences"},
	{27.0101, "Mathematics, General"},
	{27.0501, "Statistics, General"},
	{30.1301, "Medieval and Renaissance Studies"},
	{30.2001, "International/Global Studies"},
	{30.2201, "Ancient Studies/Civilization"},
	{30.2202, "Classical, Ancient Mediterranean Studies"},
	{30.9999, "Multi-/Interdisciplinary Studies"},
	{31.0504, "Sport and Fitness Administration"},
	{31.0505, "Kinesiology and Exercise Science"},
	{38.0101, "Philosophy"},
	{38.0206, "Jewish/Judaic Studies"},
	{40.0201, "Astronomy"},
	{40.0501, "Chemistry, General"},
	{40.0601, "Geology/Earth Science, General"},
	{40.0607, "Oceanography, Chemical and Physics"},
	{40.0699, "Geological and Earth Sciences"},
	{40.0801, "Physics, General"},
	{42.2704, "Experimental Psychology"},
	{42.2706, "Physiological Psychology/Psychology"},
	{44.0501, "Public Policy Analysis"},
	{45.0101, "Social Sciences, General"},
	{45.0201, "Anthropology"},
	{45.0299, "Anthropology, Other"},
	{45.0601, "Economics, General"},
	{45.1001, "Political Science and Government"},
	{45.1101, "Sociology"},
	{50.0301, "Dance, General"},
	{50.0501, "Drama and Dramatics/Theatre Arts"},
	{50.0502, "Technical Theatre/Theatre Design"},
	{50.0509, "Musical Theatre."},
	{50.0601, "Film/Cinema Studies"},
	{50.0701, "Art/Art Studies, General"},
	{50.0703, "Art History, Criticism"},
	{50.0901, "Music, General"},
	{50.0902, "Music History, Literature"},
	{50.0903, "Music Performance, General"},
	{50.0904, "Music Theory and Composition"},
	{50.091, "Jazz/Jazz Studies"},
	{50.0913, "Music Technology."},
	{50.0999, "Music, Other"},
	{50.9999, "Visual and Performing Arts, Other"},
	{51.0602, "Dental Hygiene/Hygienist"},
	{51.0913, "Athletic Training/Trainer"},
	{51.201, "Pharmaceutical Sciences."},
	{51.3801, "Registered Nursing/Registered"},
	{52.0201, "Business Administration and Marketing"},
	{52.0799, "Entrepreneurial and Small Business"},
	{52.1003, "Organizational Behavior Studies"},
	{54.0101, "History, General"},
};
