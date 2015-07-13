%module student_container

%include "exception.i"
%include "std_string.i"
%include "std_vector.i"
%include "student.i"

%{
#include "student_container.hpp"
%}


// Swig doesn't recognize std::exception, so it is ignored.
class StudentNotFound /* : public std::exception */ {
    StudentNotFound(Student::Id id);
	const char* what() const;
};


%exception StudentContainer::Find {
    try {
        $action
    } catch (StudentNotFound& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

namespace std {
    %template(StudentVector) vector<Student>;
}


class StudentContainer {
 public:
    typedef std::vector<Student> container_t;

	static StudentContainer LoadFromArchive(std::string input_path);
    // Only expose const member function to external clients.
	virtual const Student& Find(Student::Id id) const;
	void UpdateCourses(const CourseContainer& courses);

    container_t::const_iterator begin() const;
    container_t::const_iterator end() const;

 protected:
    StudentContainer();
};
