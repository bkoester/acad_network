#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <boost/program_options.hpp>

#include "course_container.hpp"
#include "course_network.hpp"
#include "mem_usage.hpp"
#include "reduce_network.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"


using std::copy_if; using std::for_each;
using std::begin; using std::end; using std::back_inserter;
using std::cerr; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string; using std::to_string;
using std::vector;

namespace po = boost::program_options;


static void SaveIndividualStudentNetwork(const StudentNetwork& network, 
		StudentNetwork::vertex_t student_d, const string& file_name);


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for saving individual networks:"};
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

	vector<Student> musical_theater;
		copy_if(begin(students), end(students), back_inserter(musical_theater),
				[](const Student& student) {
					return student.GetMajor1Description() == 
						"Musical Theatre."; });

		vector<Student> general_studies;
		copy_if(begin(students), end(students), back_inserter(musical_theater),
				[](const Student& student) {
					return student.GetMajor1Description() == 
						"General Studies"; });

		vector<Student> philosophy;
		copy_if(begin(students), end(students), back_inserter(musical_theater),
				[](const Student& student) {
					return student.GetMajor1Description() == 
						"Philosophy"; });


		for_each(begin(philosophy), end(philosophy),
				[&student_network](const Student& student) {
					SaveIndividualStudentNetwork(
							student_network, 
							student_network.GetVertexDescriptor(student.id()), 
							"output/philosophy_" + to_string(student.id()) + 
								".tsv"); });

		for_each(begin(general_studies), end(general_studies),
				[&student_network](const Student& student) {
					SaveIndividualStudentNetwork(
							student_network, 
							student_network.GetVertexDescriptor(student.id()), 
							"output/general_studies_" + 
								to_string(student.id()) + ".tsv"); });

		for_each(begin(musical_theater), end(musical_theater),
				[&student_network](const Student& student) {
					SaveIndividualStudentNetwork(
							student_network,
							student_network.GetVertexDescriptor(student.id()),
							"output/musical_theatre_" + 
								to_string(student.id()) + ".tsv"); });

	return 0;
}


/* Saves an indidivual student's network to the given file in the form:
 * <connected student 1>\t<edge weight of connected student 1>
 * <connected student 2>\t<edge weight of connected student 2>
 * ...
 * <connected student n>\t<edge weight of connected student n>
 */
void SaveIndividualStudentNetwork(const StudentNetwork& network, 
		StudentNetwork::vertex_t student_d, const string& file_name) {
	ofstream individual_network{file_name};
	for (const auto& edge_d : network.GetOutEdgeDescriptors(student_d)) {
		individual_network << network.GetTargetValue(edge_d) << "\t"
						   << network[edge_d] << endl;
	}
}



