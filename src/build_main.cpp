#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "graph_builder.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::istream; using std::ofstream; using std::ostream;
using std::string;
namespace po = boost::program_options;


enum class NetworkToBuild_e { Course, Student };

static ostream& operator<<(
		ostream& output, const NetworkToBuild_e& network_to_build);
static istream& operator>>(istream& input, NetworkToBuild_e& network_to_build);


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for thesis binary"};
	string student_path, enrollment_path;
	NetworkToBuild_e network_to_build;
	desc.add_options()
		("help,h", "Show this help message")
		("student_file", po::value<string>(&student_path)->required(),
		 "Set the path at which to find the student file")
		("enrollment_file", po::value<string>(&enrollment_path)->required(),
		 "Set the path at which to find the enrollment file")
		("network_to_build", 
		 po::value<NetworkToBuild_e>(&network_to_build)->default_value(
			 NetworkToBuild_e::Student), "Set the network to build "
		 "('student' or 'course')")
		("threads,t", 
		 po::value<int>(&num_network_building_threads)->default_value(1),
		 "Number of threads to use to build the network");

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

	if (network_to_build == NetworkToBuild_e::Course) {
		// build the course network
		CourseNetwork course_network{
			BuildCourseNetworkFromEnrollment(enrollment_stream)};
		course_network.Save(cout);
	} else {
		assert(network_to_build == NetworkToBuild_e::Student);

		// build the student network
		student_container_t students{ReadStudents(student_stream)};
		course_container_t courses{ReadEnrollment(enrollment_stream, students)};
		StudentNetwork student_network{BuildStudentNetworkFromStudents(students)};
		student_network.Save(cout);
	}

	return 0;
}


ostream& operator<<(
		ostream& output, const NetworkToBuild_e& network_to_build) {
	if (network_to_build == NetworkToBuild_e::Course)
	{ output << "Course"; }
	else if (network_to_build == NetworkToBuild_e::Student)
	{ output << "Student"; }
	else { assert(false); }
	return output;
}


istream& operator>>(istream& input, NetworkToBuild_e& network_to_build) {
	// get the string
	string network_type;
	input >> network_type;

	// make sure the network type is valid, throw error if not
	if (icompare(network_type, "course"))
	{ network_to_build = NetworkToBuild_e::Course; }
	else if (icompare(network_type, "student"))
	{ network_to_build = NetworkToBuild_e::Student; }
	else { throw po::invalid_option_value{"Invalid network type!"}; }

	return input;
}
