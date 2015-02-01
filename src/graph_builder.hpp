#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <iosfwd>

#include "tab_reader.hpp"


class CourseNetwork;
class StudentNetwork;


// The number of threads to help build the network. Defined as extern to allow
// change by command line options.
extern int num_network_building_threads;


// Builds a graph for the network from the given course tab.
CourseNetwork BuildCourseNetworkFromEnrollment(std::istream& enrollment_stream);

StudentNetwork BuildStudentNetworkFromEnrollment(
		std::istream& enrollment_stream);

StudentNetwork BuildStudentNetworkFromStudents(
		const student_container_t& students);

#endif  // GRAPH_BUILDER_H


