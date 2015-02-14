#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "student_network.hpp"
#include "utility.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string;
namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for network building binary:"};
	NetworkType_e network_to_load;
	string course_archive_path, student_archive_path;
	desc.add_options()
		("help,h", "Show this help message")
		("course_archive", po::value<string>(&course_archive_path),
		 "Set the path at which to find the archived course network")
		("student_archive", po::value<string>(&student_archive_path),
		 "Set the path at which to find the archive student network")
		("network_to_load", 
		 po::value<NetworkType_e>(&network_to_load)->default_value(
			 NetworkType_e::Student), "Set the network to load "
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
		// Make sure the appropriate input file is given.
		if (network_to_load == NetworkType_e::Student && 
				student_archive_path.empty()) {
			throw po::required_option{"Must provide student archive file!"}; 
		} else if (network_to_load == NetworkType_e::Course &&
				course_archive_path.empty()) {
			throw po::required_option{"Must provide course archive file!"};
		}
		po::notify(vm);
	} catch (po::required_option& e) {
		cerr << e.what() << endl;
		return -1;
	} catch (po::error& e) {
		cerr << e.what() << endl;
		return -1;
	}

	// Do whatever work necessary
	if (network_to_load == NetworkType_e::Course) {
		ifstream course_archive{course_archive_path};
		CourseNetwork course_network{course_archive};
	}
	if (network_to_load == NetworkType_e::Student) {
		ifstream student_archive{student_archive_path};
		StudentNetwork student_network{student_archive};
		// output all edges
		for (const auto& edge_d : student_network.GetEdgeDescriptors()) {
			cout << student_network.GetSourceValue(edge_d) << "\t"
				 << student_network.GetTargetValue(edge_d) << "\t"
				 << student_network[edge_d] << endl;
		}
		// output weighted and unweighted degree for every vertex
		for (const auto& vertex_d : student_network.GetVertexDescriptors()) {
			auto out_edges = student_network.GetOutEdgeValues(vertex_d);
			auto weighted_degree = accumulate(
					begin(out_edges), end(out_edges), 0.);
			auto unweighted_degree = out_edges.size();
			cout << student_network[vertex_d] << "\t" << unweighted_degree 
				 << "\t" << weighted_degree << endl;
		}
	}

	return 0;
}
