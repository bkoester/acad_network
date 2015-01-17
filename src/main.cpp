#include <iostream>
#include <fstream>

#include "course_network.hpp"
#include "graph_builder.hpp"


using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 


int main() {
	/*
	CourseNetwork::graph_t graph{BuildCourseGraphFromTab(cin)};
	CourseNetwork course_network{graph};
	ofstream output_archive{"/tmp/course_network.txt"};
	course_network.Save(output_archive);
	*/

	/*
	StudentNetwork::graph_t graph{BuildStudentGraphFromTab(cin)};
	StudentNetwork student_network{graph};
	ofstream output_archive{"/tmp/student_network.txt"};
	student_network.Save(output_archive);
	*/

	ifstream input_archive{"../../data/course_network.txt"};
	CourseNetwork course_network{input_archive};
	for (const auto& vertex : course_network.GetVertices()) {
		auto edge_it_pair = course_network.GetEdgeIterators(vertex);
		int weighted_degree{0};
		for (auto edge_it = edge_it_pair.first; edge_it != edge_it_pair.second;
				++edge_it) { weighted_degree += course_network[*edge_it]; }
		cout << course_network[vertex] << " " << course_network.Degree(vertex) 
			 << " " << weighted_degree << endl;
	}

	return 0;
}
