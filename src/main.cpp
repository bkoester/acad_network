#include <iostream>

#include "course_network.hpp"
#include "graph_builder.hpp"

using std::cin;

int main() {
	CourseNetwork::graph_t graph{BuildGraphFromCourseTab(cin)};
	CourseNetwork course_network{graph};
	course_network.Save("/tmp/course_network.txt");
	return 0;
}
