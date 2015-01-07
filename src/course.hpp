#ifndef COURSE_H
#define COURSE_H

#include <functional>
#include <iosfwd>
#include <string>

class Course {

 public:
	Course();

	Course(std::string subject, int number) :
		subject_{subject}, number_{number} {}

	std::string subject() const { return subject_; }
	int number() const { return number_; }

	bool operator==(const Course& other) const
	{ return other.subject_ == subject_ && other.number_ == number_; }

	bool operator!=(const Course& other) const { return !(*this == other); }

	bool operator<(const Course& other) const;

 private:
	std::string subject_;
	int number_;
};

std::ostream& operator<<(std::ostream& output, const Course& course);

struct CourseHasher {
	int operator()(const Course& course) const {
		return std::hash<std::string>()(course.subject()) ^
			std::hash<int>()(course.number());
	}
};


#endif  // COURSE_H
