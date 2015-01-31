#include "course_tab.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "utility.hpp"


using std::istream; using std::ostream; using std::cerr; using std::endl;
using std::string;
using std::stringstream;


CourseTab::CourseTab(istream& course_tab) : course_tab_(course_tab) {
	// skip the headings line
	SkipLine(course_tab_);
}


CourseTab::Iterator CourseTab::begin() { return Iterator{course_tab_}; }


CourseTab::Iterator CourseTab::end() { return Iterator{course_tab_, true}; }


CourseTab::Iterator::Iterator(istream& course_tab) :
	course_tab_(course_tab), end_{false} { operator++(); }


CourseTab::Iterator::Iterator(istream& course_tab, bool end) :
	course_tab_(course_tab), end_{end} {}


CourseTab::Iterator::Iterator(const Iterator& other) : 
	course_tab_(other.course_tab_), end_{other.end_}, line_{other.line_} {}


CourseTab::Iterator CourseTab::Iterator::operator++(int) { 
	Iterator copy{*this};
	Advance();
	return copy;
}


CourseTab::Iterator& CourseTab::Iterator::operator++() { 
	Advance();
	return *this;
}


bool CourseTab::Iterator::operator==(const Iterator& other) const {
	return (end_ == true && other.end_ == true) || 
		   (line_ == other.line_ && end_ == other.end_);
}


bool CourseTab::Iterator::operator!=(const Iterator& other) const
{ return !(operator==(other)); }


void CourseTab::Iterator::Advance() {
	// declare variables
	string line, subject;
	int student_id, course_number;

	// get next line while we haven't hit the end and we get empty lines
	while (line.empty() && !(end_ = (getline(course_tab_, line)).eof()));
	if (end_) { return; }
	stringstream student_stream{line};
	student_stream >> student_id >> subject >> course_number;
	if (!student_stream) { throw BadCourseTabError{}; }

	// store line state
	line_.course = Course{subject, course_number};
	line_.student = StudentId{student_id};
}


CourseTab::Line& CourseTab::Iterator::operator*() { return line_; }


CourseTab::Line* CourseTab::Iterator::operator->() { return &line_; }


ostream& operator<<(ostream& os, const CourseTab::Iterator& it) {
	os << "End: " << it.end_ << ", Line: " << it.line_;
	return os;
}


ostream& operator<<(ostream& os, const CourseTab::Line& line) {
	os << "Student: " << line.student << ", Course: " << line.course;
	return os;
}
