#ifndef STUDENT_H
#define STUDENT_H

#include <functional>
#include <iosfwd>
#include <initializer_list>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/mpl/bool.hpp>
#include <boost/optional.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/utility.hpp>

#include "utility.hpp"


class Course;
struct CourseComparator;


class Student {
 public:
	using Id = int;
	using IdHasher = std::hash<int>;

	enum class Gender { Male, Female, Unspecified };
	enum class Ethnicity { Hispanic_only, American_Indian, Asian, Black, 
		Pacific_Islander, White, Multiracial, Unknown, Undocumented };

	static std::string to_string(const Gender& gender);
	static std::string to_string(const Ethnicity& ethnicity);

	Student() : id_{uninitialized_id}, gender_{Gender::Unspecified}, 
		ethnicity_{uninitialized_ethnicity},
		first_term_{uninitialized_term} {}
	explicit Student(Student::Id id) : id_{id}, gender_{Gender::Unspecified}, 
		ethnicity_{uninitialized_ethnicity},
		first_term_{uninitialized_term} {}
	Student(Student::Id id, Gender gender, Ethnicity ethnicity, int first_term,
			int degree_term, bool transfer, std::string school) : 
		id_{id}, gender_{gender}, ethnicity_{ethnicity}, first_term_{first_term},
		degree_term_{degree_term}, transfer_{transfer}, school_{school} {}

	int id() const { return id_; }
	Gender gender() const { return gender_; }
	Ethnicity ethnicity() const { return ethnicity_; }
	int first_term() const { return first_term_; }
	int degree_term() const { return degree_term_; }
	bool transfer() const { return transfer_; }
	const boost::optional<double>& major1() const { return major1_; }
	const boost::optional<double>& major2() const { return major2_; }
	std::string school() const { return school_; }

	std::string GetMajor1Description() const;
	std::string GetMajor2Description() const;

	bool operator==(const Student& other) const { return id() == other.id(); }
	bool operator!=(const Student& other) const { return !(operator==(other)); }
	bool operator<(const Student& other) const { return id() < other.id(); }

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version);

	void AddCourseTaken(const Course* course) { courses_taken_.insert(course); }
	void AddCoursesTaken(std::initializer_list<const Course*> courses);
	bool HasTakenCourse(const Course* course) const
	{ return courses_taken_.count(course) == 1; }
	// provide access to the container to allow stl algorithms to be run on them
	const std::set<const Course*>& courses_taken() const
	{ return courses_taken_; }

	double GetTotalCreditsTaken() const;

 private:
	friend class StudentTest;
	friend class boost::serialization::access;
	friend std::ostream& operator<<(std::ostream& os, const Student& student);
	friend std::istream& operator>>(std::istream& input, Student& student);

	Id id_;
	Gender gender_;
	Ethnicity ethnicity_;
	int first_term_;
	int degree_term_;
	bool transfer_;
	boost::optional<double> major1_;
	boost::optional<double> major2_;
	std::string school_;
	std::set<const Course*> courses_taken_;
	static const int uninitialized_id;
	static const int uninitialized_term;
	static const Ethnicity uninitialized_ethnicity;
};


std::istream& operator>>(std::istream& input, Student::Gender& gender);
std::ostream& operator<<(std::ostream& output, const Student::Gender& gender);
std::istream& operator>>(std::istream& input, Student::Ethnicity& ethnicity);
std::ostream& operator<<(
		std::ostream& output, const Student::Ethnicity& ethnicity);


template <typename Archive>
void Student::serialize(Archive& ar, const unsigned int) {
	ar & id_;
	ar & first_term_;
	ar & degree_term_;
	ar & transfer_;
	ar & major1_;
	ar & major2_;
	ar & school_;
	// do not serialize courses taken

	// perform different for enums depending on whether we're saving or loading
	if (Archive::is_saving::value) {
		auto gender_underlying_type = ToIntegralType(gender());
		auto ethnicity_underlying_type = ToIntegralType(ethnicity());
		ar & gender_underlying_type;
		ar & ethnicity_underlying_type;
	} else if (Archive::is_loading::value) {
		std::underlying_type<Gender>::type gender_underlying;
		std::underlying_type<Ethnicity>::type ethnicity_underlying;

		ar & gender_underlying;
		gender_ = static_cast<Gender>(gender_underlying);
		ar & ethnicity_underlying;
		ethnicity_ = static_cast<Ethnicity>(ethnicity_underlying);
	}
}

struct StudentHasher {
	size_t operator()(const Student& student) const 
	{ return std::hash<int>()(student.id()); }
};


#endif  // STUDENT_H
