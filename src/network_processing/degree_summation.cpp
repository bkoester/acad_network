#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <boost/program_options.hpp>

#include "course_container.hpp"
#include "reduce_network.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"


using std::begin; using std::end;
using std::cerr; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string;

namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Sum degrees of students in network:"};
	string student_archive_path, course_archive_path, 
		   course_network_archive_path, student_network_archive_path;
	desc.add_options()
		("help,h", "Show this help message")
		("student_network_archive_path",
		 po::value<string>(&student_network_archive_path)->required(),
		 "Set the path at which to find the archive student network")
		("student_archive_path", 
		 po::value<string>(&student_archive_path)->required(),
		 "Set the path at which to find the student file")
		("course_archive_path", 
		 po::value<string>(&course_archive_path)->required(),
		 "Set the path at which to find the enrollment file");

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

	// read students and enrollment data
	ifstream student_archive{student_archive_path};
	ifstream course_archive{course_archive_path};
    StudentContainer students{
        StudentContainer::LoadFromArchive(student_archive)};
	CourseContainer courses{
        CourseContainer::LoadFromArchive(course_archive)};
	students.UpdateCourses(courses);

	ifstream student_network_archive{student_network_archive_path};
	StudentNetwork student_network{student_network_archive};

	ofstream weighted_students{"output/student_weighted_summation.tsv"};
	ofstream unweighted_students{"output/student_unweighted_summation.tsv"};
	for (const auto& student_d : student_network.GetVertexDescriptors()) {
		auto out_edges = student_network.GetOutEdgeValues(student_d);
		auto weighted_sum = accumulate(
				begin(out_edges), end(out_edges), 0.);
		auto unweighted_sum = out_edges.size();
		weighted_students << student_network[student_d] << '\t' << weighted_sum
						  << endl;
		unweighted_students << student_network[student_d] << '\t' 
							<< unweighted_sum << endl;
	}
}
