%module student_container
%include "std_string.i"
%include "student.i"
%{
#include "student_container.hpp"
%}

class StudentContainer {
 public:
	static StudentContainer LoadFromArchive(std::string input_path);
    // Only expose const member function to external clients.
	virtual const Student& Find(Student::Id id) const;

 protected:
    StudentContainer();
};
