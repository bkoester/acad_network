#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/program_options.hpp>

#include "course_container.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"


using std::transform;
using std::begin; using std::back_inserter; using std::end;
using std::cerr; using std::cout; using std::endl;
using std::ifstream; using std::ofstream;
using std::pair;
using std::string; using std::to_string;
using std::vector;

namespace po = boost::program_options;
using boost::algorithm::join;


template <typename FilterFunc>
static void SaveWeightedDistances(const StudentNetwork& network,
								   const string& filename,
								   FilterFunc filter);

template <typename FilterFunc>
static void SaveUnweightedDistances(const StudentNetwork& network,
									 const string& filename,
									 FilterFunc filter);


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for saving individual distances:"};
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

	SaveWeightedDistances(
			student_network,
			"musical_theatre_weighted_distances.tsv",
			[&students](Student::Id id)
			{ return students.Find(id).GetMajor1Description() ==
			"Musical Theatre."; });

	SaveUnweightedDistances(
			student_network,
			"musical_theatre_unweighted_distances.tsv",
			[&students](Student::Id id)
			{ return students.Find(id).GetMajor1Description() ==
			"Musical Theatre."; });

	SaveWeightedDistances(
			student_network,
			"general_studies_weighted_distances.tsv",
			[&students](Student::Id id)
			{ return students.Find(id).GetMajor1Description() ==
			"General Studies"; });


	SaveUnweightedDistances(
			student_network,
			"general_studies_unweighted_distances.tsv",
			[&students](Student::Id id)
			{ return students.Find(id).GetMajor1Description() ==
			"General Studies"; });

	SaveWeightedDistances(
			student_network,
			"philosophy_weighted_distances.tsv",
			[&students](Student::Id id)
			{ return students.Find(id).GetMajor1Description() ==
			"Philosophy"; });

	SaveUnweightedDistances(
			student_network,
			"philosophy_unweighted_distances.tsv",
			[&students](Student::Id id)
			{ return students.Find(id).GetMajor1Description() ==
			"Philosophy"; });

	return 0;
}

/* Gets dijkstra shortest path for a set of vertices to every other (connected)
 * vertex on the graph and writes it to stats_dijkstra.tsv. Applies a filter to
 * determine if it should compute for the given student.
 * FilterFunc should be a function that accepts a student ID and returns a
 * boolean value for whether shortest paths should be found for that student.
 */
template <typename FilterFunc>
void SaveWeightedDistances(const StudentNetwork& network,
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

/* Gets unweighted shortest path for a set of vertices to every other
 * (connected) vertex on the graph and writes it to stats_dijkstra.tsv. Applies
 * a filter to determine if it should compute for the given student. FilterFunc
 * should be a function that accepts a student ID and returns a boolean value
 * for whether shortest paths should be found for that student.
 */
template <typename FilterFunc>
void SaveUnweightedDistances(const StudentNetwork& network,
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
