%module course_container

%include "course.i"
%include "std_string.i"
%include "std_vector.i"

%{
#include "course_container.hpp"
%}


namespace std {
    %template(CourseVector) vector<Course>;
}


class CourseContainer {
 public:
    typedef std::vector<Course> container_t;

	static CourseContainer LoadFromArchive(std::string input_path);
    // Only expose const member function to external clients.
	const Course& Find(Course course) const;

    container_t::const_iterator begin() const;
    container_t::const_iterator end() const;

 protected:
    CourseContainer();
};
