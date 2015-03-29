#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <iosfwd>

#include "tab_reader.hpp"


class CourseNetwork;
class StudentNetwork;


// Builds a graph for the network from the given course tab.
CourseNetwork BuildCourseNetworkFromEnrollment(std::istream& enrollment_stream);

StudentNetwork BuildStudentNetworkFromEnrollment(
		std::istream& enrollment_stream);

StudentNetwork BuildStudentNetworkFromStudents(
		const Student::container_t& students);

#endif  // GRAPH_BUILDER_H


