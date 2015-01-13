#include <iostream>
#include <fstream>

#include "course_network.hpp"
#include "graph_builder.hpp"


using std::cin;
using std::ofstream;


int main() {
	/*
	CourseNetwork::graph_t graph{BuildCourseGraphFromTab(cin)};
	CourseNetwork course_network{graph};
	ofstream output_archive{"/tmp/course_network.txt"};
	course_network.Save(output_archive);
	*/

	StudentNetwork::graph_t graph{BuildStudentGraphFromTab(cin)};
	StudentNetwork student_network{graph};
	ofstream output_archive{"/tmp/student_network.txt"};
	student_network.Save(output_archive);

	return 0;
}
