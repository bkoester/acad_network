%module course

%include "std_string.i"

%{
#include "course.hpp"
%}

// workaround for lack of swig support of nested classes
struct CourseId {
    Id();
    Id(std::string s, short n, int t);

    std::string subject;
    short number;
    int term;
};


class Course {
 public:
    Course();
	Course(std::string subject, short number, int term);

    std::string subject() const;
	short number() const;
	int term() const;
	double num_credits() const;
};

%{
// trick C++ compiler into thinking global class is inner class of Course
typedef Course::Id CourseId;
%}
