#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <iosfwd>


class CourseNetwork;
class StudentContainer;
class StudentNetwork;


// Builds a graph for the network from the given course tab.
CourseNetwork BuildCourseNetworkFromEnrollment(const StudentContainer& students);

StudentNetwork BuildStudentNetworkFromEnrollment(
		std::istream& enrollment_stream);

StudentNetwork BuildStudentNetworkFromStudents(
		const StudentContainer& students);

#endif  // GRAPH_BUILDER_H


