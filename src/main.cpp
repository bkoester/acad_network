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

	StudentNetwork student_network{BuildStudentGraphFromTab(cin)};
	student_network.Save(cout);

	/*
	ifstream input_archive{"../../data/course_network.txt"};
	CourseNetwork course_network{input_archive};
	for (const auto& edge : course_network.GetEdges()) {
		auto edge_it_pair = course_network.GetEdgeIterators(vertex);
		int weighted_degree{0};
		for (auto edge_it = edge_it_pair.first; edge_it != edge_it_pair.second;
				++edge_it) { weighted_degree += course_network[*edge_it]; }
		cout << course_network[vertex] << " " << course_network.Degree(vertex) 
			 << " " << weighted_degree << endl;
		cout << course_network.GetSourceVertex(edge) << "\t"
			 << course_network.GetTargetVertex(edge) << "\t"
			 << course_network[edge] << endl;
	}
	*/

	return 0;
}
