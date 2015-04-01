#include <iostream>
#include <fstream>
#include <string>

#include <boost/program_options.hpp>

#include "course.hpp"
#include "student.hpp"
#include "tab_reader.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream;
using std::string;
namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for student segmenting binary:"};
	string student_path, enrollment_path;
	desc.add_options()
		("help,h", "Show this help message")
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
	} catch (po::error& e) {
		cerr << e.what() << endl;
		return -1;
	}

	// read students and enrollment data
	ifstream student_stream{student_path};
	ifstream enrollment_stream{enrollment_path};
	Student::container_t students{ReadStudents(student_stream)};
	Course::container_t courses{ReadEnrollment(enrollment_stream, students)};

	while (!cin.eof()) {
		cout << "Enter student ID > ";
		Student::Id input_id{0};
		while (cin >> input_id) {
			try {
				cout << FindStudent(input_id, students) << endl;
			} catch (StudentNotFound& student_not_found) {
				cerr << student_not_found.what() << endl;
			}
		}
		if (cin.fail() && !cin.eof()) { 
			cout << "You must input a student ID!" << endl;
			cin.clear();
		}
	}
}
