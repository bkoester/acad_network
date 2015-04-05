#include "student.hpp"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include "course.hpp"
#include "utility.hpp"


using std::begin; using std::end;
using std::cout; using std::endl;
using std::copy; using std::transform; using std::remove;
using std::ostream; using std::istream; using std::endl;
using std::ostringstream;
using std::ostream_iterator; using std::inserter;
using std::string; using std::stoi;
using std::unordered_map;


const unordered_map<double, string> major_code_map{
	{30103, "Environmental Studies"},
	{40201, "Architecture (BArch, BA/BS, MA"},
	{50102, "American/United States Studies"},
	{50103, "Asian Studies/Civilization"},
	{50108, "Near and Middle Eastern Studies"},
	{5011, "Russian Studies"},
	{50134, "Latin American and Caribbean Studies"},
	{50201, "African-American/Black Studies"},
	{50203, "Hispanic-American, Puerto Rican Studies"},
	{50207, "Women's Studies"},
	{90101, "Communication Studies/Speech Communication"},
	{110101, "Computer and Information Science"},
	{110104, "Informatics."},
	{131202, "Elementary Education and Teaching"},
	{131205, "Secondary Education and Teaching"},
	{131312, "Music Teacher Education"},
	{131314, "Physical Education Teaching"},
	{140101, "Engineering, General"},
	{140201, "Aerospace, Aeronautical"},
	{140501, "Biomedical/Medical Engineering"},
	{140701, "Chemical Engineering"},
	{140801, "Civil Engineering, General"},
	{140901, "Computer Engineering, General"},
	{141001, "Electrical, Electronics and Computer Engineering"},
	{141201, "Engineering Physics"},
	{141401, "Environmental"},
	{141801, "Materials Engineering"},
	{141901, "Mechanical Engineering"},
	{142201, "Naval Architecture and Marine"},
	{142301, "Nuclear Engineering"},
	{143501, "Industrial Engineering"},
	{143901, "Geological/Geophysical Enginee"},
	{149999, "Engineering, Other"},
	{160102, "Linguistics"},
	{160104, "Comparative Literature"},
	{160402, "Russian Language and Literature"},
	{160407, "Polish Language and Literature"},
	{160501, "German Language and Literature"},
	{160601, "Modern Greek Language and Literature"},
	{1609, "Romance Languages, Literatures"},
	{160901, "French Language and Literature"},
	{160902, "Italian Language and Literature"},
	{160905, "Spanish Language and Literature"},
	{161102, "Hebrew Language and Literature"},
	{161199, "Middle/Near Eastern and Semitic"},
	{1612, "Classics and Languages, Literature"},
	{161202, "Ancient/Classical Greek Language"},
	{161203, "Latin Language and Literature"},
	{230101, "English Language and Literature"},
	{231302, "Creative Writing."},
	{240102, "General Studies"},
	{240103, "Humanities/Humanistic Studies"},
	{260101, "Biology/Biological Sciences, General"},
	{260202, "Biochemistry"},
	{260203, "Biophysics"},
	{260204, "Molecular Biology"},
	{260406, "Cell/Cellular and Molecular Biology"},
	{260502, "Microbiology, General"},
	{26131, "Ecology and Evolutionary Biology"},
	{261501, "Neuroscience."},
	{269999, "Biological and Biomedical Sciences"},
	{270101, "Mathematics, General"},
	{270501, "Statistics, General"},
	{301301, "Medieval and Renaissance Studies"},
	{302001, "International/Global Studies"},
	{302201, "Ancient Studies/Civilization"},
	{302202, "Classical, Ancient Mediterranean Studies"},
	{309999, "Multi-/Interdisciplinary Studies"},
	{310504, "Sport and Fitness Administration"},
	{310505, "Kinesiology and Exercise Science"},
	{380101, "Philosophy"},
	{380206, "Jewish/Judaic Studies"},
	{400201, "Astronomy"},
	{400501, "Chemistry, General"},
	{400601, "Geology/Earth Science, General"},
	{400607, "Oceanography, Chemical and Physics"},
	{400699, "Geological and Earth Sciences"},
	{400801, "Physics, General"},
	{422704, "Experimental Psychology"},
	{422706, "Physiological Psychology/Psychology"},
	{440501, "Public Policy Analysis"},
	{450101, "Social Sciences, General"},
	{450201, "Anthropology"},
	{450299, "Anthropology, Other"},
	{450601, "Economics, General"},
	{451001, "Political Science and Government"},
	{451101, "Sociology"},
	{500301, "Dance, General"},
	{500501, "Drama and Dramatics/Theatre Arts"},
	{500502, "Technical Theatre/Theatre Design"},
	{500509, "Musical Theatre."},
	{500601, "Film/Cinema Studies"},
	{500701, "Art/Art Studies, General"},
	{500703, "Art History, Criticism"},
	{500901, "Music, General"},
	{500902, "Music History, Literature"},
	{500903, "Music Performance, General"},
	{500904, "Music Theory and Composition"},
	{50091, "Jazz/Jazz Studies"},
	{500913, "Music Technology."},
	{500999, "Music, Other"},
	{509999, "Visual and Performing Arts, Other"},
	{510602, "Dental Hygiene/Hygienist"},
	{510913, "Athletic Training/Trainer"},
	{51201, "Pharmaceutical Sciences."},
	{513801, "Registered Nursing/Registered"},
	{520201, "Business Administration and Marketing"},
	{520799, "Entrepreneurial and Small Business"},
	{521003, "Organizational Behavior Studies"},
	{540101, "History, General"},
};


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
	os << "; School: " << student.school();

	// print first term
	os << "; First term: ";
	if (student.first_term() == Student::uninitialized_term)
	{ os << "Undefined"; }
	else { os << student.first_term(); }
	os << "; Major(s): ";
	if (student.major1()) { os << student.GetMajor1Description(); }
	if (student.major2()) { os << ", " << student.GetMajor2Description(); }
	if (!student.courses_taken_.empty()) {
		os << "; Classes: ";
		transform(student.courses_taken_.cbegin(),
				  --student.courses_taken_.cend(),
				  ostream_iterator<Course::Id>{os, ", "},
				  [](const Course* course) { return course->GetId(); });
		os << (*student.courses_taken_.rbegin())->GetId();
		os << endl;
	}
	return os;
}


istream& operator>>(istream& input, Student& student) {
	// read the input data we're interested in, skip the rest of the line
	Student::Id id;
	Student::Gender gender;
	Student::Ethnicity ethnicity{Student::Ethnicity::Unknown};
	int first_term, degree_term;
	char transfer;
	string major1, major2;

	input >> id >> gender >> ethnicity >> first_term >> degree_term >> transfer
		  >> major1 >> major2;
	if (!input) { return input; }

	SkipTabField(input);  // pre-major1 description tab
	SkipTabField(input);  // pre-major2 description tab
	SkipTabField(input);  // pre-first declare code tab
	SkipTabField(input);  // pre-first declare description tab
	SkipTabField(input);  // pre-first declare term tab
	SkipTabField(input);  // pre-pell status tab
	SkipTabField(input);  // pre-ACT english tab
	SkipTabField(input);  // pre-ACT math tab
	SkipTabField(input);  // pre-ACT comp tab
	SkipTabField(input);  // post-ACT comp tab

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
	// remove "." from numbers so they become integers
	major1.erase(remove(begin(major1), end(major1), '.'), end(major1));
	major2.erase(remove(begin(major2), end(major2), '.'), end(major2));
	if (major1 != "NA") { student.major1_ = stoi(major1); }
	else { student.major1_ = boost::none; }
	if (major2 != "NA") { student.major2_ = stoi(major2); }
	else { student.major2_ = boost::none; }
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


std::string Student::to_string(const Gender& gender) {
	ostringstream out;
	out << gender;
	return out.str();
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


std::string Student::to_string(const Ethnicity& ethnicity) {
	ostringstream out;
	out << ethnicity;
	return out.str();
}


const int Student::uninitialized_id{-1};
const int Student::uninitialized_term{-1};
const Student::Ethnicity Student::uninitialized_ethnicity{
	Student::Ethnicity::Unknown};
