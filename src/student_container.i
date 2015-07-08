%module student_container

%include "exception.i"
%include "std_string.i"
%include "student.i"

%{
#include "student_container.hpp"
%}


/*
class StudentNotFound : public std::exception {
    StudentNotFound(Student::Id id);
	const char* what() const;
}; */


//%catches(StudentNotFound) StudentContainer::Find(Student::Id);
/*
%exception{
    try {
        $action
    } catch (StudentNotFound& e) {
        SWIG_Python_Raise(SWIG_NewPointerObj(
            (new StudentNotFound(e),
            SWIGTYPE_p_StudentNotFound, SWIG_POINTER_OWN),
            "StudentNotFound", SWIGTYPE_p_StudentNotFound);
        SWIG_fail;
    }
} */


class StudentContainer {
 public:
	static StudentContainer LoadFromArchive(std::string input_path);
    // Only expose const member function to external clients.
	virtual const Student& Find(Student::Id id) const;
	void UpdateCourses(const CourseContainer& courses);

 protected:
    StudentContainer();
};
