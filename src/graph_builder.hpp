#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include "course_network.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"

// Builds a graph for the network from the given course tab.
CourseNetwork BuildCourseGraphFromTab(std::istream& course_tab_stream);

StudentNetwork BuildStudentGraphFromTab(std::istream& course_tab_stream);

StudentNetwork BuildStudentGraphFromEnrollment(
		const student_container_t& students);

#endif  // GRAPH_BUILDER_H


