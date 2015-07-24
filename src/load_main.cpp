#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/program_options.hpp>

#include "course_container.hpp"
#include "course_network.hpp"
#include "mem_usage.hpp"
#include "reduce_network.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"


using std::copy_if; using std::for_each; using std::transform;
using std::begin; using std::end; using std::back_inserter;
using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::pair;
using std::string; using std::to_string;
using std::unique_ptr;
using std::vector;

namespace po = boost::program_options;
using boost::algorithm::join;


/*
static void GetWeightedAndUnweightedSummations(const StudentNetwork& network);

static void PrintIndividualStudentNetwork(const StudentNetwork& network, 
		StudentNetwork::vertex_t student_d, const string& file_name);
*/

template <typename FilterFunc>
static void PrintWeightedDistances(const StudentNetwork& network, 
									 const string& filename,
									 FilterFunc filter);

template <typename FilterFunc>
static void PrintUnweightedDistances(const StudentNetwork& network,
									   const string& filename,
									   FilterFunc filter);

/*
static void ReduceStudentNetwork(const StudentNetwork& network,
								 const StudentContainer& students,
								 const CourseContainer& courses);

template <typename SegmentFunc, typename AccumulateFunc, typename Init>
static void MakeReducedNetwork(const StudentNetwork& network, string segment, 
							   string weightedness, SegmentFunc segment_func, 
							   AccumulateFunc accumulate_func, 
							   Init init);
*/


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for network loading binary:"};
	NetworkType_e network_to_load;
	string student_archive_path, course_archive_path, 
		   course_network_archive_path, student_network_archive_path;
	desc.add_options()
		("help,h", "Show this help message")
		("course_network_archive_path", 
		 po::value<string>(&course_network_archive_path),
		 "Set the path at which to find the archived course network")
		("student_network_archive_path",
		 po::value<string>(&student_network_archive_path),
		 "Set the path at which to find the archive student network")
		("student_archive_path", 
		 po::value<string>(&student_archive_path)->required(),
		 "Set the path at which to find the student file")
		("course_archive_path", 
		 po::value<string>(&course_archive_path)->required(),
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

	// read students and enrollment data
	ifstream student_archive{student_archive_path};
	ifstream course_archive{course_archive_path};
    StudentContainer students{
        StudentContainer::LoadFromArchive(student_archive)};
	CourseContainer courses{
        CourseContainer::LoadFromArchive(course_archive)};
	students.UpdateCourses(courses);
	
	// Do whatever work necessary
	if (network_to_load == NetworkType_e::Course) {
		ifstream course_archive{course_archive_path};
		CourseNetwork course_network{course_archive};
	}
	if (network_to_load == NetworkType_e::Student) {
		ifstream student_network_archive{student_network_archive_path};
		StudentNetwork student_network{student_network_archive};
	
		PrintWeightedDistances(
				student_network, 
				"musical_theatre_weighted_distances.tsv",
				[&students](Student::Id id) 
				{ return students.Find(id).GetMajor1Description() == 
				"Musical Theatre."; });

		PrintUnweightedDistances(
				student_network, 
				"musical_theatre_unweighted_distances.tsv",
				[&students](Student::Id id) 
				{ return students.Find(id).GetMajor1Description() == 
				"Musical Theatre."; });

		PrintWeightedDistances(
				student_network, 
				"general_studies_weighted_distances.tsv",
				[&students](Student::Id id) 
				{ return students.Find(id).GetMajor1Description() == 
				"General Studies"; });


		PrintUnweightedDistances(
				student_network, 
				"general_studies_unweighted_distances.tsv",
				[&students](Student::Id id) 
				{ return students.Find(id).GetMajor1Description() == 
				"General Studies"; });

		PrintWeightedDistances(
				student_network, 
				"philosophy_weighted_distances.tsv",
				[&students](Student::Id id) 
				{ return students.Find(id).GetMajor1Description() == 
				"Philosophy"; });

		PrintUnweightedDistances(
				student_network, 
				"philosophy_unweighted_distances.tsv",
				[&students](Student::Id id) 
				{ return students.Find(id).GetMajor1Description() == 
				"Philosophy"; });

		/*
		ReduceStudentNetwork(student_network, students, courses);
		
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
					PrintIndividualStudentNetwork(
							student_network, 
							student_network.GetVertexDescriptor(student.id()), 
							"output/individual_" + to_string(student.id()) + 
								".tsv"); });

		for_each(begin(general_studies), end(general_studies),
				[&student_network](const Student& student) {
					PrintIndividualStudentNetwork(
							student_network, 
							student_network.GetVertexDescriptor(student.id()), 
							"output/individual_" + to_string(student.id()) + 
								".tsv"); });

		for_each(begin(musical_theater), end(musical_theater),
				[&student_network](const Student& student) {
					PrintIndividualStudentNetwork(
							student_network,
							student_network.GetVertexDescriptor(student.id()),
							"output/individual_" + to_string(student.id()) + 
								".tsv"); });
		*/
	}

	return 0;
}


/*
void GetWeightedAndUnweightedSummations(const StudentNetwork& network) {
	ofstream weighted_students{"output/student_weighted_summation.tsv"};
	ofstream unweighted_students{"output/student_unweighted_summation.tsv"};
	for (const auto& student_d : network.GetVertexDescriptors()) {
		auto out_edges = network.GetOutEdgeValues(student_d);
		auto weighted_sum = accumulate(
				begin(out_edges), end(out_edges), 0.);
		auto unweighted_sum = out_edges.size();
		weighted_students << network[student_d] << '\t' << weighted_sum << endl;
		unweighted_students << network[student_d] << '\t' 
							<< unweighted_sum << endl;
	}
} */


void PrintIndividualStudentNetwork(const StudentNetwork& network, 
		StudentNetwork::vertex_t student_d, const string& file_name) {
	ofstream individual_network{file_name};
	for (const auto& edge_d : network.GetOutEdgeDescriptors(student_d)) {
		individual_network << network.GetTargetValue(edge_d) << "\t"
						   << network[edge_d] << endl;
	}
}


// Gets dijkstra shortest path for a set of vertices to every other (connected)
// vertex on the graph and writes it to stats_dijkstra.tsv. Applies a filter to
// determine if it should compute for the given student.
// FilterFunc should be a function that accepts a student ID and returns a
// boolean value for whether shortest paths should be found for that student.
template <typename FilterFunc>
void PrintWeightedDistances(const StudentNetwork& network, 
							const string& filename, 
							FilterFunc filter) {
	ofstream dijkstra_file{filename};
	for (auto vertex_d : network.GetVertexDescriptors()) {
		auto student_id = network[vertex_d];

		// Determine whether or not we should process this student.
		if (!filter(student_id)) { continue; }

		dijkstra_file << student_id << "\t";

		// weighted distance stats
		auto weighted_distances = network.FindWeightedDistances(vertex_d);

		// get the second member of the pair
		auto distance_values = vector<string>{};
		transform(begin(weighted_distances), end(weighted_distances),
				back_inserter(distance_values), 
				[](pair<Student::Id, double> elt)
				{ return to_string(elt.second); });
		dijkstra_file << join(distance_values, "\t") << endl;
	}
}


template <typename FilterFunc>
void PrintUnweightedDistances(const StudentNetwork& network, 
							  const string& filename,
							  FilterFunc filter) {
	ofstream bfs_file{filename};
	for (auto vertex_d : network.GetVertexDescriptors()) {
		auto student_id = network[vertex_d];

		// Determine whether or not we should process this student.
		if (!filter(student_id)) { continue; }

		bfs_file << network[vertex_d] << "\t";

		// unweighted distance stats
		auto unweighted_distances = network.FindUnweightedDistances(vertex_d);

		// get the second member of the pair
		auto distance_values = vector<string>{};
		transform(begin(unweighted_distances), end(unweighted_distances),
				back_inserter(distance_values),
				[](pair<Student::Id, double> elt)
				{ return to_string(elt.second); });
		bfs_file << join(distance_values, "\t") << endl;
	}
}

/*
// reduces network to see the interaction between various segments
void ReduceStudentNetwork(const StudentNetwork& network,
						  const StudentContainer& students,
						  const CourseContainer& courses) {
	auto weighted_func = [](double edge, double current_edge) 
				{ return edge + current_edge; };
	auto unweighted_func = [](double edge, int current_edge) 
				{ return 1 + current_edge; };

	// major
	auto major1_func = [&students](const Student::Id& id)
		{ return students.Find(id).GetMajor1Description(); };
	MakeReducedNetwork(
			network, "major1", "weighted", major1_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "major1", "unweighted", major1_func, unweighted_func, 0);
	
	// school
	auto school_func = [&students](const Student::Id& id)
		{ return students.Find(id).school(); };
	auto weighted_school_network = ReduceNetwork(
			network, school_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "school", "weighted", school_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "school", "unweighted", school_func, unweighted_func, 0);

	// ethnicity
	auto ethnicity_func = [&students](const Student::Id& id)
		{ return students.Find(id).ethnicity(); };
	auto weighted_ethnicity_network = ReduceNetwork(
			network, ethnicity_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "ethnicity", "weighted", ethnicity_func, weighted_func, 0.);
	MakeReducedNetwork(network, "ethnicity", "unweighted", ethnicity_func, 
					   unweighted_func, 0);
}

template <typename SegmentFunc, typename AccumulateFunc, typename Init>
void MakeReducedNetwork(const StudentNetwork& network, string segment, 
						string weightedness, SegmentFunc segment_func, 
						AccumulateFunc accumulate_func, 
						Init init) {
	auto reduced_network = ReduceNetwork(
			network, segment_func, accumulate_func, init);
	ofstream output{"output/network_" + segment + "_" + weightedness + ".tsv"};
	output << segment << "1" << '\t' << segment << "2" << '\t' << "count" 
		   << endl;
	reduced_network.SaveEdgewise(output);
} */
