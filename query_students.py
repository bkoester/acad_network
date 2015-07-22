"""Utility to segment students."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import sys

from course_container_wrapper import CourseContainerWrapper
import readable_directory
from student_container_wrapper import StudentContainerWrapper


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Output information about '
        'students')
    parser.add_argument('--student-archive-path', dest='student_archive_path',
        help='Path of the student archive file.', required=True)
    parser.add_argument('--course-archive-path', dest='course_archive_path',
        help='Path of the course archive file.', required=True)
    parser.add_argument('--swig-module-path', dest='swig_module_path', 
        help='Directory containing the swig modules.',
        action=readable_directory.ReadableDirectory, required=True)
    
    args = parser.parse_args()

    # import the course_container and student_container modules
    students_wrapper = StudentContainerWrapper(
            args.swig_module_path, args.student_archive_path)
    courses_wrapper = CourseContainerWrapper(
            args.swig_module_path, args.course_archive_path)
    students_wrapper.load_courses(courses_wrapper.courses)

    # read integer IDs of the students for which to print information
    while True:
        try:
            student_id = int(input('Enter student ID: '))
            s = students_wrapper.students.Find(student_id)
            print(s.GetDescription())
        except ValueError:
            print('Please enter a valid integer!')
        except RuntimeError as e:
            print(e, end='\n\n')

