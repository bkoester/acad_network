#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string;
namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for network loading binary:"};
	NetworkType_e network_to_load;
	string course_archive_path, student_archive_path, 
		   student_path, enrollment_path;
	desc.add_options()
		("help,h", "Show this help message")
		("course_archive", po::value<string>(&course_archive_path),
		 "Set the path at which to find the archived course network")
		("student_archive", po::value<string>(&student_archive_path),
		 "Set the path at which to find the archive student network")
		("student_file", po::value<string>(&student_path)->required(),
		 "Set the path at which to find the student file")
		("enrollment_file", po::value<string>(&enrollment_path)->required(),
		 "Set the path at which to find the enrollment file")
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
		po::notify(vm);

		// Make sure the appropriate input file is given.
		if (network_to_load == NetworkType_e::Student && 
				student_archive_path.empty()) {
			throw po::required_option{"student_archive"}; 
		} else if (network_to_load == NetworkType_e::Course &&
				course_archive_path.empty()) {
			throw po::required_option{"course_archive"};
		}
	} catch (po::required_option& e) {
		cerr << e.what() << endl;
		return -1;
	} catch (po::error& e) {
		cerr << e.what() << endl;
		return -1;
	}

	// Read students and enrollment data.
	ifstream student_stream{student_path};
	ifstream enrollment_stream{enrollment_path};
	student_container_t students{ReadStudents(student_stream)};
	course_container_t courses{ReadEnrollment(enrollment_stream, students)};

	// Do whatever work necessary
	if (network_to_load == NetworkType_e::Course) {
		ifstream course_archive{course_archive_path};
		CourseNetwork course_network{course_archive};
	}
	if (network_to_load == NetworkType_e::Student) {
		ifstream student_archive{student_archive_path};
		StudentNetwork student_network{student_archive};

		// Create a bunch of output streams.
		ofstream gender_weighted{"output/gender_weighted.tab"};
		ofstream gender_unweighted{"output/gender_unweighted.tab"};
		ofstream term_weighted{"output/term_weighted.tab"};
		ofstream term_unweighted{"output/term_unweighted.tab"};
		ofstream ethnicity_weighted{"output/ethnicity_weighted.tab"};
		ofstream ethnicity_unweighted{"output/ethnicity_unweighted.tab"};
		ofstream transfer_weighted{"output/transfer_weighted.tab"};
		ofstream transfer_unweighted{"output/transfer_unweighted.tab"};

		// output weighted and unweighted degree for every vertex
		for (const auto& vertex_d : student_network.GetVertexDescriptors()) {
			auto out_edges = student_network.GetOutEdgeValues(vertex_d);
			auto weighted_degree = accumulate(
					begin(out_edges), end(out_edges), 0.);
			auto unweighted_degree = out_edges.size();
			auto student = FindStudent(student_network[vertex_d], students);

			// Output information to the correct files.
			gender_unweighted << student.gender() << "\t" << unweighted_degree 
							  << endl;;
			gender_weighted << student.gender() << "\t" << weighted_degree 
							<< endl;
			term_unweighted << student.first_term() << "\t" << unweighted_degree
							<< endl;
			term_weighted << student.first_term() << "\t" << weighted_degree
						  << endl;
			ethnicity_unweighted << student.ethnicity() << "\t" 
								 << unweighted_degree << endl;
			ethnicity_weighted << student.ethnicity() << "\t" 
							   << weighted_degree << endl;
			transfer_unweighted << student.transfer() << "\t" 
								<< unweighted_degree << endl;
			transfer_weighted << student.transfer() << "\t" 
							  << weighted_degree << endl;
		}
	}

	return 0;
}
