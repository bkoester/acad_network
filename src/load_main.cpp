#include <fstream>
#include <iostream>
#include <string>

#include "course_network.hpp"
#include "student_network.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string;


int main(int argc, char* argv[]) {
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
