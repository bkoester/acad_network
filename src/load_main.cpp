#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <boost/program_options.hpp>

#include "course_network.hpp"
#include "reduce_network.hpp"
#include "student_network.hpp"
#include "student_segmentation.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::ofstream; 
using std::string; using std::to_string;
using std::unique_ptr;
namespace po = boost::program_options;


static void SegmentStudents(const StudentNetwork& network,
							const Student::container_t& students, 
							const Course::container_t& courses);

static void ReduceStudentNetwork(const StudentNetwork& network,
								 const Student::container_t& students,
								 const Course::container_t& courses);

template <typename SegmentFunc, typename AccumulateFunc, typename Init>
static void MakeReducedNetwork(const StudentNetwork& network, string segment, 
							   string weightedness, SegmentFunc segment_func, 
							   AccumulateFunc accumulate_func, 
							   Init init);



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
		
		SegmentStudents(student_network, students, courses);
		ReduceStudentNetwork(student_network, students, courses);
	}

	return 0;
}

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
}


// segments students according to various attributes and puts information in
// separate output files
void SegmentStudents(const StudentNetwork& network,
					 const Student::container_t& students, 
					 const Course::container_t& courses) {
	using VertexData = StudentSegmenter::VertexData;
	StudentSegmenter segmenter;

	auto weighted_func = [](const VertexData& data)
		{ return to_string(data.weighted_degree); };
	auto weighted_norm_func = [](const VertexData& data)
		{ return to_string(data.weighted_degree_norm); };
	auto unweighted_func = [](const VertexData& data)
		{ return to_string(data.unweighted_degree); };

	// gender
	ofstream gender_weighted{"output/gender_weighted.tsv"};
	ofstream gender_weighted_norm{"output/gender_weighted_norm.tsv"};
	ofstream gender_unweighted{"output/gender_unweighted.tsv"};
	auto gender_func = [](const Student& student) 
		{ return Student::to_string(student.gender()); };
	segmenter.AddSegment(gender_weighted, gender_func, weighted_func);
	segmenter.AddSegment(gender_weighted_norm, gender_func, weighted_norm_func);
	segmenter.AddSegment(gender_unweighted, gender_func, unweighted_func);

	// term
	ofstream term_weighted{"output/term_weighted.tsv"};
	ofstream term_weighted_norm{"output/term_weighted_norm.tsv"};
	ofstream term_unweighted{"output/term_unweighted.tsv"};
	auto first_term_func = [](const Student& student)
		{ return to_string(student.first_term()); };
	segmenter.AddSegment(term_weighted, first_term_func, weighted_func);
	segmenter.AddSegment(
			term_weighted_norm, first_term_func, weighted_norm_func);
	segmenter.AddSegment(term_unweighted, first_term_func, unweighted_func);

	// ethnicity
	ofstream ethnicity_weighted{"output/ethnicity_weighted.tsv"};
	ofstream ethnicity_weighted_norm{"output/ethnicity_weighted_norm.tsv"};
	ofstream ethnicity_unweighted{"output/ethnicity_unweighted.tsv"};
	auto ethnicity_func = [](const Student& student)
		{ return Student::to_string(student.ethnicity()); };
	segmenter.AddSegment(ethnicity_weighted, ethnicity_func, weighted_func);
	segmenter.AddSegment(
			ethnicity_weighted_norm, ethnicity_func, weighted_norm_func);
	segmenter.AddSegment(ethnicity_unweighted, ethnicity_func, unweighted_func);

	// transfer
	ofstream transfer_weighted{"output/transfer_weighted.tsv"};
	ofstream transfer_weighted_norm{"output/transfer_weighted_norm.tsv"};
	ofstream transfer_unweighted{"output/transfer_unweighted.tsv"};
	auto transfer_func = 
		[](const Student& student)
		{ return student.transfer() ? "Transfer" : "Non-transfer"; };
	segmenter.AddSegment(transfer_weighted, transfer_func, weighted_func);
	segmenter.AddSegment(
			transfer_weighted_norm, transfer_func, weighted_norm_func);
	segmenter.AddSegment(transfer_unweighted, transfer_func, unweighted_func);

	// major 1
	ofstream major1_weighted{"output/major1_weighted.tsv"};
	ofstream major1_weighted_norm{"output/major1_weighted_norm.tsv"};
	ofstream major1_unweighted{"output/major1_unweighted.tsv"};
	auto major1_func = [](const Student& student)
		{ return student.GetMajor1Description(); };
	segmenter.AddSegment(major1_weighted, major1_func, weighted_func);
	segmenter.AddSegment(major1_weighted_norm, major1_func, weighted_norm_func);
	segmenter.AddSegment(major1_unweighted, major1_func, unweighted_func);

	// major 2
	ofstream major2_weighted{"output/major2_weighted.tsv"};
	ofstream major2_weighted_norm{"output/major2_weighted_norm.tsv"};
	ofstream major2_unweighted{"output/major2_unweighted.tsv"};
	auto major2_func = [](const Student& student)
		{ return student.GetMajor2Description(); };
	segmenter.AddSegment(major2_weighted, major2_func, weighted_func);
	segmenter.AddSegment(major2_weighted_norm, major2_func, weighted_norm_func);
	segmenter.AddSegment(major2_unweighted, major2_func, unweighted_func);

	// major status
	ofstream major_status_weighted{"output/major_status_weighted.tsv"};
	ofstream major_status_weighted_norm{
		"output/major_status_weighted_norm.tsv"};
	ofstream major_status_unweighted{"output/major_status_unweighted.tsv"};
	auto major_status = [](const Student& student) {
		auto major_status = string{"Undeclared"};
		if (bool(student.major1()) != bool(student.major2())) {
			major_status = "One major"; 
		} else if (bool(student.major1()) && bool(student.major2())) {
			major_status = "Double major";
		}
		return major_status;
	};
	segmenter.AddSegment(major_status_weighted, major_status, weighted_func);
	segmenter.AddSegment(
			major_status_weighted_norm, major_status, weighted_norm_func);
	segmenter.AddSegment(
			major_status_unweighted, major_status, unweighted_func);

	// school
	ofstream school_weighted{"output/school_weighted.tsv"};
	ofstream school_weighted_norm{"output/school_weighted_norm.tsv"};
	ofstream school_unweighted{"output/school_unweighted.tsv"};
	auto school_func = [](const Student& student) { return student.school(); };
	segmenter.AddSegment(school_weighted, school_func, weighted_func);
	segmenter.AddSegment(school_weighted_norm, school_func, weighted_norm_func);
	segmenter.AddSegment(school_unweighted, school_func, unweighted_func);

	segmenter.RunSegmentation(network, students);
}
