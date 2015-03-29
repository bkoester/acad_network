#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "reduce_network.hpp"
#include "student_network.hpp"
#include "student_segmentation.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


using std::begin; using std::end; using std::ostream_iterator;
using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::pair;
using std::string; using std::to_string;
using std::unique_ptr;
namespace po = boost::program_options;


static void ComputeStudentStats(const StudentNetwork& network);

/*
static void ReduceStudentNetwork(const StudentNetwork& network,
								 const Student::container_t& students,
								 const Course::container_t& courses);

template <typename SegmentFunc, typename AccumulateFunc, typename Init>
static void MakeReducedNetwork(const StudentNetwork& network, string segment, 
							   string weightedness, SegmentFunc segment_func, 
							   AccumulateFunc accumulate_func, 
							   Init init);
							   */



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
	Student::container_t students{ReadStudents(student_stream)};
	Course::container_t courses{ReadEnrollment(enrollment_stream, students)};

	// Do whatever work necessary
	if (network_to_load == NetworkType_e::Course) {
		ifstream course_archive{course_archive_path};
		CourseNetwork course_network{course_archive};
	}
	if (network_to_load == NetworkType_e::Student) {
		ifstream student_archive{student_archive_path};
		StudentNetwork student_network{student_archive};
	
		ComputeStudentStats(student_network);
		//ReduceStudentNetwork(student_network, students, courses);
	}

	return 0;
}

void ComputeStudentStats(const StudentNetwork& network) {
	ofstream dijkstra_file{"output/stats_dijkstra.tsv"};
	ofstream bfs_file{"output/stats_bfs.tsv"};
	for (auto vertex_d : network.GetVertexDescriptors()) {
		dijkstra_file << network[vertex_d] << "\t";
		bfs_file << network[vertex_d] << "\t";
		// find distances to other vertices
		auto unweighted_distances = network.FindUnweightedDistances(vertex_d);
		ostream_iterator<int> bfs_it{bfs_file, " "};
		transform(begin(unweighted_distances), end(unweighted_distances), 
				bfs_it, [](pair<Student::Id, int> elt)
				{ return elt.second; });
		bfs_file << endl;
		
		auto weighted_distances = network.FindWeightedDistances(vertex_d);
		ostream_iterator<double> dijkstra_it{dijkstra_file, " "};
		transform(begin(weighted_distances), end(weighted_distances), 
				dijkstra_it, [](pair<Student::Id, double> elt)
				{ return elt.second; });
		dijkstra_file << endl;
	}

}


/*
// reduces network to see the interaction between various segments
void ReduceStudentNetwork(const StudentNetwork& network,
						  const Student::container_t& students,
						  const Course::container_t& courses) {
	auto weighted_func = [](double edge, double current_edge) 
				{ return edge + current_edge; };
	auto unweighted_func = [](double edge, int current_edge) 
				{ return 1 + current_edge; };

	// major
	auto major1_func = [&students](const Student::Id& id)
		{ return FindStudent(id, students).GetMajor1Description(); };
	MakeReducedNetwork(
			network, "major1", "weighted", major1_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "major1", "unweighted", major1_func, unweighted_func, 0);
	
	// school
	auto school_func = [&students](const Student::Id& id)
		{ return FindStudent(id, students).school(); };
	auto weighted_school_network = ReduceNetwork(
			network, school_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "school", "weighted", school_func, weighted_func, 0.);
	MakeReducedNetwork(
			network, "school", "unweighted", school_func, unweighted_func, 0);

	// ethnicity
	auto ethnicity_func = [&students](const Student::Id& id)
		{ return FindStudent(id, students).ethnicity(); };
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
