#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "graph_builder.hpp"
#include "mem_usage.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::istream; using std::ofstream; using std::ostream;
using std::string;

namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for network building binary:"};
	string student_path, enrollment_path;
	NetworkType_e network_to_build;
	desc.add_options()
		("help,h", "Show this help message")
		("student_file", po::value<string>(&student_path)->required(),
		 "Set the path at which to find the student file")
		("enrollment_file", po::value<string>(&enrollment_path)->required(),
		 "Set the path at which to find the enrollment file")
		("network_to_build", 
		 po::value<NetworkType_e>(&network_to_build)->default_value(
			 NetworkType_e::Student), "Set the network to build "
		 "('student' or 'course')")
		("threads,t", po::value<int>(&num_threads)->default_value(1),
		 "Number of threads to use to build the network");

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		if (vm.count("help")) {
			cout << desc << endl;
			return 0;
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

	if (network_to_build == NetworkType_e::Course) {
		// build the course network
		CourseNetwork course_network{
			BuildCourseNetworkFromEnrollment(enrollment_stream)};
		course_network.Save(cout);
	} else {
		assert(network_to_build == NetworkType_e::Student);

		// build the student network
		student_container_t students{ReadStudents(student_stream)};
		course_container_t courses{ReadEnrollment(enrollment_stream, students)};
		cerr << "Mem usage: " << GetMemoryUsage() << endl;
		StudentNetwork student_network{
			BuildStudentNetworkFromStudents(students)};
		student_network.Save(cout);
	}

	return 0;
}
