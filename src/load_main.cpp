#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
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
#include "student_segmentation.hpp"
#include "utility.hpp"


using std::copy_if; using std::for_each;
using std::begin; using std::end; using std::back_inserter;
using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::ostream_iterator;
using std::pair;
using std::string; using std::to_string;
using std::unique_ptr;
using std::vector;
namespace po = boost::program_options;
namespace chr = std::chrono;



//static void GetWeightedAndUnweightedSummations(const StudentNetwork& network);

static void PrintIndividualStudentNetwork(const StudentNetwork& network, 
		StudentNetwork::vertex_t student_d, const string& file_name);

/*
static void ComputeWeightedDistances(const StudentNetwork& network);
static void ComputeUnweightedDistances(const StudentNetwork& network);
							   */

static void ReduceStudentNetwork(const StudentNetwork& network,
								 const StudentContainer& students,
								 const CourseContainer& courses);

template <typename SegmentFunc, typename AccumulateFunc, typename Init>
static void MakeReducedNetwork(const StudentNetwork& network, string segment, 
							   string weightedness, SegmentFunc segment_func, 
							   AccumulateFunc accumulate_func, 
							   Init init);


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
	StudentContainer students;
	students.Load(student_archive);
	CourseContainer courses;
	courses.Load(course_archive);
	students.UpdateCourses(courses);
	
	// Do whatever work necessary
	if (network_to_load == NetworkType_e::Course) {
		ifstream course_archive{course_archive_path};
		CourseNetwork course_network{course_archive};
	}
	if (network_to_load == NetworkType_e::Student) {
		ifstream student_network_archive{student_network_archive_path};
		StudentNetwork student_network{student_network_archive};
	
		//ComputeWeightedDistances(student_network);
		//ComputeUnweightedDistances(student_network);
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
							"output/philosophy_" + to_string(student.id()) + 
								".tsv"); });

		for_each(begin(general_studies), end(general_studies),
				[&student_network](const Student& student) {
					PrintIndividualStudentNetwork(
							student_network, 
							student_network.GetVertexDescriptor(student.id()), 
							"output/general_studies_" + 
								to_string(student.id()) + ".tsv"); });

		for_each(begin(musical_theater), end(musical_theater),
				[&student_network](const Student& student) {
					PrintIndividualStudentNetwork(
							student_network,
							student_network.GetVertexDescriptor(student.id()),
							"output/musical_theatre_" + 
								to_string(student.id()) + ".tsv"); });
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


/*
void ComputeWeightedDistances(const StudentNetwork& network) {
	ofstream dijkstra_file{"output/stats_dijkstra.tsv"};
	auto beginning_time = chr::system_clock::now();
	int num_students{0};
	for (auto vertex_d : network.GetVertexDescriptors()) {
		dijkstra_file << network[vertex_d] << "\t";

		// weighted distance stats
		auto weighted_distances = network.FindWeightedDistances(vertex_d);
		ostream_iterator<double> dijkstra_it{dijkstra_file, " "};
		transform(begin(weighted_distances), end(weighted_distances), 
				dijkstra_it, [](pair<Student::Id, double> elt)
				{ return elt.second; });
		dijkstra_file << endl;

		// output timing information
		if (++num_students % 10 == 0) {
			cerr << num_students << "\t" << chr::duration_cast<chr::seconds>(
				chr::system_clock::now() - beginning_time).count() << endl;
		}
		if (num_students > 500) { break; }
	}
}


void ComputeUnweightedDistances(const StudentNetwork& network) {
	ofstream bfs_file{"output/stats_bfs.tsv"};
	auto beginning_time = chr::system_clock::now();
	int num_students{0};
	for (auto vertex_d : network.GetVertexDescriptors()) {
		bfs_file << network[vertex_d] << "\t";

		// unweighted distance stats
		auto unweighted_distances = network.FindUnweightedDistances(vertex_d);
		ostream_iterator<int> bfs_it{bfs_file, " "};
		transform(begin(unweighted_distances), end(unweighted_distances), 
				bfs_it, [](pair<Student::Id, int> elt)
				{ return elt.second; });
		bfs_file << endl;
		
		// output timing information
		if (++num_students % 10 == 0) {
			cerr << num_students << "\t" << chr::duration_cast<chr::seconds>(
				chr::system_clock::now() - beginning_time).count() << endl;
		}
		if (num_students > 500) { break; }
	}
}
*/

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
}
