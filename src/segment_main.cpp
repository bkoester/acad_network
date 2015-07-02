#include <iostream>
#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

#include <boost/program_options.hpp>

#include "course.hpp"
#include "course_container.hpp"
#include "student.hpp"
#include "student_container.hpp"
#include "utility.hpp"


using std::cout; using std::cerr; using std::cin; using std::endl; 
using std::getline; using std::string; 
using std::ifstream;
using std::function;
using std::move;
using std::ostream;
using std::stringstream;
using std::unique_ptr;
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
	{"transfer", [](ostream& os, const Student& student)
		{ os << (student.transfer() ? "Transfer" : "Non-transfer"); }},
	{"first_year", [](ostream& os, const Student& student)
		{ os << student.first_term() / 100; }},
};


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for student segmenting binary:"};
	string student_archive_path, course_archive_path, segment, input_path;
	unique_ptr<ifstream> given_input_file;  // owns input file if path given
	auto input_stream = &cin;  // pointer to the input stream to use
	desc.add_options()
		("help,h", "Show this help message")
		("segment,s", po::value<string>(&segment)->required(),
		 "Field by which to segment data")
		("input_file", po::value<string>(&input_path),
		 "File that contains data to be segmented")
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

		if (!input_path.empty()) {
			given_input_file = ::make_unique<ifstream>(input_path);
			input_stream = given_input_file.get();
		}

		if (!segmenters.count(segment))
		{ throw po::invalid_option_value{segment}; }

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

	// read data to segment from the input stream
	auto segmentation_func = segmenters.find(segment)->second;
	for (string line; getline(*input_stream, line);) {
		stringstream line_stream{line};
		auto field_num = 0;
		for (string field; getline(line_stream, field, '\t');) {
			if (field_num++ != 0) { 
				cout << '\t' << field; 
				break;
			}
			Student::Id student_id{stoi(field)};
			segmentation_func(cout, students.Find(student_id));
		}
		cout << endl;
	}
}
