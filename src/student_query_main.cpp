#include <iostream>
#include <fstream>
#include <string>

#include <boost/program_options.hpp>

#include "course.hpp"
#include "course_container.hpp"
#include "student.hpp"
#include "student_container.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream;
using std::string;
namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for student segmenting binary:"};
	string student_archive_path, course_archive_path;
	desc.add_options()
		("help,h", "Show this help message")
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

	cout << "Students: " << students.size() << endl;
	cout << "Courses: " << courses.size() << endl;

	while (!cin.eof()) {
		cout << "Enter student ID > ";
		Student::Id input_id{0};
		while (cin >> input_id) {
			try {
				cout << students.Find(input_id) << endl;
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
