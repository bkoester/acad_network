#include <iostream>
#include <fstream>
#include <string>

#include <boost/program_options.hpp>

#include "course.hpp"
#include "course_container.hpp"
#include "student.hpp"
#include "student_container.hpp"

using std::cerr; using std::cout; using std::endl;
using std::ifstream; using std::ofstream;
using std::string;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
	po::options_description desc{"Save archives of students and courses:"};
	string student_path, enrollment_path, student_archive_path, 
		   course_archive_path;
	desc.add_options()
		("help,h", "Show this help message")
		("student_file", po::value<string>(&student_path)->required(),
		 "Set the path at which to find the student file")
		("enrollment_file", po::value<string>(&enrollment_path)->required(),
		 "Set the path at which to find the enrollment file")
		("student_archive_path", 
		 po::value<string>(&student_archive_path)->required(),
		 "Set the path to which the student archive should be saved.")
		("course_archive_path", 
		 po::value<string>(&course_archive_path)->required(),
		 "Set the path at which to course archive should be saved.");

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
	StudentContainer students{StudentContainer::LoadFromTsv(student_stream)};
	CourseContainer courses{CourseContainer::LoadFromTsv(enrollment_stream)};

	// save archives of students and courses
	ofstream student_archive{student_archive_path};
	ofstream course_archive{course_archive_path};
	students.SaveToArchive(student_archive);
	courses.SaveToArchive(course_archive);
}
