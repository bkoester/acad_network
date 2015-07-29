#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_container.hpp"
#include "reduce_network.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"


using std::cerr; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string; using std::to_string;

namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for reducing network:"};
	string student_archive_path, course_archive_path, 
		   course_network_archive_path, student_network_archive_path;
	desc.add_options()
		("help,h", "Show this help message")
		("student_network_archive_path",
		 po::value<string>(&student_network_archive_path),
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

	auto weighted_func = [](double edge, double current_edge) 
				{ return edge + current_edge; };
	auto unweighted_func = [](double, int current_edge) 
				{ return 1 + current_edge; };

	// major
	auto major1_func = [&students](const Student::Id& id)
		{ return students.Find(id).GetMajor1Description(); };

	auto major1_weighted_reduced_network = ReduceNetwork(
			student_network, major1_func, weighted_func, 0.);
	ofstream major1_weighted_output{"output/network_major1_weighted.tsv"};
	major1_weighted_reduced_network.SaveEdgewise(major1_weighted_output);

	auto major1_unweighted_reduced_network = ReduceNetwork(
			student_network, major1_func, unweighted_func, 0);
	ofstream major1_unweighted_output{"output/network_major1_unweighted.tsv"};
	major1_unweighted_reduced_network.SaveEdgewise(major1_unweighted_output);
		
	// school
	auto school_func = [&students](const Student::Id& id)
		{ return students.Find(id).school(); };

	auto school_weighted_student_network = ReduceNetwork(
			student_network, school_func, weighted_func, 0.);
	ofstream school_weighted_output{"output/network_school_weighted.tsv"};
	school_weighted_student_network.SaveEdgewise(school_weighted_output);

	auto school_unweighted_student_network = ReduceNetwork(
			student_network, school_func, unweighted_func, 0);
	ofstream school_unweighted_output{"output/network_school_unweighted.tsv"};
	school_unweighted_student_network.SaveEdgewise(school_unweighted_output);

	// ethnicity
	auto ethnicity_func = [&students](const Student::Id& id)
		{ return students.Find(id).ethnicity(); };

	auto ethnicity_weighted_student_network = ReduceNetwork(
			student_network, ethnicity_func, weighted_func, 0.);
	ofstream ethnicity_weighted_output{"output/network_ethnicity_weighted.tsv"};
	ethnicity_weighted_student_network.SaveEdgewise(ethnicity_weighted_output);

	auto ethnicity_unweighted_student_network = ReduceNetwork(
			student_network, ethnicity_func, unweighted_func, 0.);
	ofstream ethnicity_unweighted_output{
		"output/network_ethnicity_unweighted.tsv"};
	ethnicity_unweighted_student_network.SaveEdgewise(
			ethnicity_unweighted_output);

	return 0;
}
