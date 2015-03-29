#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>

#include <boost/program_options.hpp>
#define CSV_IO_NO_THREAD // turn off threading for the csv library
#include "third_party/csv.hpp"

#include "course.hpp"
#include "student.hpp"
#include "tab_reader.hpp"
#include "utility.hpp"


using std::cout; using std::cerr; using std::cin; using std::endl; 
using std::ifstream;
using std::function;
using std::ostream;
using std::string;
using std::unordered_map;
namespace po = boost::program_options;


const unordered_map<string, function<void(ostream&, const Student&)>> 
segmenters{
	{"gender", [](ostream& os, const Student& student)
		{ os << student.gender(); }},
	{"ethnicity", [](ostream& os, const Student& student)
		{ os << student.ethnicity(); }},
	{"major1", [](ostream& os, const Student& student)
		{ os << student.GetMajor1Description(); }},
	{"school", [](ostream& os, const Student& student)
		{ os << student.school(); }},
};


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for student segmenting binary:"};
	string segment, student_path, enrollment_path, segment_path;
	desc.add_options()
		("help,h", "Show this help message")
		("segment,s", po::value<string>(&segment)->required(),
		 "Field by which to segment data")
		("segment_file", po::value<string>(&segment_path)->required(),
		 "File that contains data to be segmented")
		("student_file", po::value<string>(&student_path)->required(),
		 "Set the path at which to find the student file")
		("enrollment_file", po::value<string>(&enrollment_path)->required(),
		 "Set the path at which to find the enrollment file");

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		if (vm.count("help")) {
			cout << desc << endl;
			return 0;
		}
		po::notify(vm);

		if (!segmenters.count(segment))
		{ throw po::invalid_option_value{segment}; }

	} catch (po::error& e) {
		cerr << e.what() << endl;
		return -1;
	}

	// read students and enrollment data
	ifstream student_stream{student_path};
	ifstream enrollment_stream{enrollment_path};
	Student::container_t students{ReadStudents(student_stream)};
	Course::container_t courses{ReadEnrollment(enrollment_stream, students)};

	// read data to segment from standard input
	io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>> reader{
		segment_path};
	reader.set_header("student_id", "data");
	Student::Id student_id{0};
	string data;
	auto segmentation_func = segmenters.find(segment)->second;
	while (reader.read_row(student_id, data)) {
		segmentation_func(cout, student_id);
		cout << data << endl;
	}
}
