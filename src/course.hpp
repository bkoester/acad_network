#ifndef COURSE_H
#define COURSE_H

#include <functional>
#include <iosfwd>
#include <set>
#include <string>

#include <boost/serialization/utility.hpp>


class Student;


// An ID object for the course that uniquely identifies it.
struct CourseId {
	CourseId();
	CourseId(std::string s, short n, int t) : subject{s}, number{n}, term{t} {}

	std::string subject;
	short number;
	int term;
};


class Course {
 public:
	Course() : subject_{undefined_subject}, number_{undefined_number},
		term_{undefined_term}, num_credits_{0} {}

	Course(std::string subject, short number, int term) :
		subject_{subject}, number_{number}, term_{term}, num_credits_{0} {}

	Course(std::string subject, short number, int term, double num_credits) :
		subject_{subject}, number_{number}, term_{term}, 
		num_credits_{num_credits} {}


	std::string subject() const { return subject_; }
	short number() const { return number_; }
	int term() const { return term_; }
	double num_credits() const { return num_credits_; }

	bool operator==(const Course& other) const {
		return other.subject_ == subject_ && other.number_ == number_ && 
			other.term_ == term_; 
	}

	bool operator!=(const Course& other) const { return !(*this == other); }
	bool operator<(const Course& other) const;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & subject_;
		ar & number_;
		ar & term_;
		ar & num_credits_;
	}

	void AddStudentEnrolled(const Student* student)
	{ students_enrolled_.insert(student); }

	bool IsStudentEnrolled(const Student* student)
	{ return students_enrolled_.count(student); }

	const std::set<const Student*>& students_enrolled() const
	{ return students_enrolled_; }

	std::set<const Student*>::size_type GetNumStudentsEnrolled() const
	{ return students_enrolled_.size(); }

	CourseId GetId() const { return {subject_, number_, term_}; }

 private:
	// unit testing class
	friend class CourseTest;
	friend class boost::serialization::access;
	// ID class
	friend class CourseId;
	// read input from student course tab
	friend std::istream& operator>>(std::istream& input, Course& course);

	std::string subject_;
	short number_;
	int term_;
	double num_credits_;
	std::set<const Student*> students_enrolled_;

	static const std::string undefined_subject;
	static const short undefined_number;
	static const int undefined_term;
};


std::ostream& operator<<(std::ostream& output, const Course& course);
std::ostream& operator<<(std::ostream& output, const CourseId& course_id);


struct CourseHasher {
	int operator()(const Course& course) const {
		return std::hash<std::string>()(course.subject()) ^
			std::hash<short>()(course.number()) ^ 
			std::hash<short>()(course.term());
	}
};


#endif  // COURSE_H
