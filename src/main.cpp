#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "graph_builder.hpp"
#include "tab_reader.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string;


int main(int argc, char* argv[]) {
	namespace po = boost::program_options;
	po::options_description desc{"Options for thesis binary"};
	string student_path, enrollment_path;
	desc.add_options()
		("help,h", "Show this help message")
		("student_file", po::value<string>(&student_path)->required(),
		 "Set the path at which to find the student file")
		("enrollment_file", po::value<string>(&enrollment_path)->required(),
		 "Set the path at which to find the enrollment file");

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		if (vm.count("help")) {
			cout << desc << endl;
			return -1;
		}
		po::notify(vm);
	} catch (po::required_option& e) {
		cerr << e.what() << endl;
		return -1;
	} catch (po::error& e) {
		cerr << e.what() << endl;
		return -1;
	}

	ifstream student_stream{student_path};
	ifstream enrollment_stream{enrollment_path};

	/*
	CourseNetwork::graph_t graph{BuildCourseGraphFromTab(cin)};
	CourseNetwork course_network{graph};
	ofstream output_archive{"/tmp/course_network.txt"};
	course_network.Save(output_archive);
	*/

	student_container_t students{ReadStudents(student_stream)};
	course_container_t courses{ReadEnrollment(enrollment_stream, students)};
	StudentNetwork student_network{BuildStudentGraphFromEnrollment(students)};
	student_network.Save(cout);

	/*
	StudentNetwork student_network{BuildStudentGraphFromTab(cin)};
	student_network.Save(cout);
	*/

	/*
	ifstream input_archive{"../../data/course_network.txt"};
	CourseNetwork course_network{input_archive};
	for (const auto& vertex : course_network.GetVertices()) {
		auto edge_it_pair = course_network.GetEdgeIterators(vertex);
		int weighted_degree{0};
		for (auto edge_it = edge_it_pair.first; edge_it != edge_it_pair.second;
				++edge_it) { weighted_degree += course_network[*edge_it]; }
		cout << course_network[vertex] << " " << course_network.Degree(vertex) 
			 << " " << weighted_degree << endl;
	} */

	return 0;
}
