#ifndef COURSE_TAB_H
#define COURSE_TAB_H

#include <iosfwd>

#include "course.hpp"
#include "student.hpp"

class BadCourseTabError{};

class CourseTab {
 public:
	CourseTab(std::istream& course_tab);

	// forward declare iterator class
	class Iterator;
	Iterator begin();
	Iterator end();

	// Every line in a CourseTab contains a course and a student.
	struct Line {
		Course course;
		StudentId student;

		Line() {}
		Line(Course c, StudentId s) : course{c}, student{s} {}
		Line(const Line& other) : course{other.course}, student{other.student} {}

		bool operator==(const Line& other) const
		{ return course == other.course && other.student == student; }
	};

	// Input iterator for CourseTab.
	class Iterator {
	 public:
		Iterator(std::istream& course_tab);
		Iterator(std::istream& course_tab, bool end);

		Iterator(const Iterator& other);
		// no assignment operator, can't assign references

		Iterator operator++(int);
		Iterator& operator++();
		
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;

		Line& operator*();
		Line* operator->();

		friend std::ostream& operator<<(std::ostream& os, const Iterator& it);

	 private:
		void Advance();

		std::istream& course_tab_;
		bool end_;
		Line line_;
	};

 private:
	std::istream& course_tab_;

};

std::ostream& operator<<(std::ostream& os, const CourseTab::Line& line);


void SkipLine(std::istream& input);


#endif  // COURSE_TAB_H
