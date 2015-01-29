#ifndef COURSE_H
#define COURSE_H

#include <functional>
#include <iosfwd>
#include <string>

#include <boost/serialization/utility.hpp>

class Course {
 public:
	Course() : subject_{undefined_subject}, number_{undefined_number} {}

	Course(std::string subject, int number) :
		subject_{subject}, number_{number} {}

	std::string subject() const { return subject_; }
	int number() const { return number_; }

	bool operator==(const Course& other) const
	{ return other.subject_ == subject_ && other.number_ == number_; }
	bool operator!=(const Course& other) const { return !(*this == other); }
	bool operator<(const Course& other) const;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & subject_;
		ar & number_;
	}

 private:
	friend class CourseTest;  // unit testing class
	friend class boost::serialization::access;
	// read input from student course tab
	friend std::istream& operator>>(std::istream& input, Course& course);

	std::string subject_;
	int number_;
	static const std::string undefined_subject;
	static const int undefined_number;
};


std::ostream& operator<<(std::ostream& output, const Course& course);


struct CourseHasher {
	int operator()(const Course& course) const {
		return std::hash<std::string>()(course.subject()) ^
			std::hash<int>()(course.number());
	}
};


#endif  // COURSE_H
