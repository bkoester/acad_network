#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <iosfwd>

#include <boost/optional.hpp>


class CourseNetwork;
class Student;
class StudentContainer;
class StudentNetwork;


// Builds a graph for the network from the given course tab.
CourseNetwork BuildCourseNetworkFromEnrollment(const StudentContainer& students);


StudentNetwork BuildStudentNetworkFromEnrollment(
		std::istream& enrollment_stream);


StudentNetwork BuildStudentNetworkFromStudents(
		const StudentContainer& students,
		boost::optional<double>(*weighting_func)(
			const Student&, const Student&));

#endif  // GRAPH_BUILDER_H


