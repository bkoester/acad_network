"""Utility to segment students."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import os
import sys


class ReadableDirectory(argparse.Action):
    """Action for acception a readable directory from argparse."""

    def __call__(self, parser, namespace, values, option_string=None):
        if not os.path.isdir(values):
            raise argparse.ArgumentTypeError('%s is not a valid path!' % values)
        if not os.access(values, os.R_OK):
            raise argparse.ArgumentTypeError('%s is not readable!' % values)
        setattr(namespace, self.dest, values)


def add_path_to_python_path(path):
    """Adds directory to python path so modules can be imported from it.

    Args:
        directory (string): Path to a directory to add to the path.
    """
    sys.path.append(path)


SEGMENTERS = {
        'gender': lambda s: s.GetGenderDescription(),
        'ethnicity': lambda s: s.GetEthnicityDescription(),
        'major1': lambda s: s.GetMajor1Description(),
        'school': lambda s: s.school(),
        'transfer': lambda s: 'Transfer' if s.transfer() else 'Non-transfer',
        'first_year': lambda s: s.first_term // 100,
        }


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Segment input student IDs.')
    parser.add_argument('-d', '--delimiter', help='Delimiter by which to '
            'separate input data if there are multiple fields (default tab'
            'character)', default='\t')
    parser.add_argument('field', help='Field by which to segment students.')
    parser.add_argument('file', nargs='?', type=argparse.FileType('r'),
        default=sys.stdin, help='File to segment (default standard input).')
    parser.add_argument('--student-archive-path', dest='student_archive_path',
        help='Path of the student archive file.', required=True)
    parser.add_argument('--course-archive-path', dest='course_archive_path',
        help='Path of the course archive file.', required=True)
    parser.add_argument('--swig-module-path', dest='swig_module_path', 
        help='Directory containing the swig modules.', action=ReadableDirectory,
        required=True)


    args = parser.parse_args()

    # import the course_container and student_container modules
    add_path_to_python_path(args.swig_module_path)
    import course_container
    import student_container

    students = student_container.StudentContainer_LoadFromArchive(
            args.student_archive_path)
    courses = course_container.CourseContainer_LoadFromArchive(
            args.course_archive_path)
    students.UpdateCourses(courses)

    # segment IDs (use the first field in the line as the ID)
    segmenter = SEGMENTERS[args.field]
    writer = csv.writer(sys.stdout, delimiter=args.delimiter)
    for line in csv.reader(args.file, delimiter=args.delimiter):
        # replace the student ID with the segment value
        student_id = line[0]
        student = students.Find(int(student_id))
        line[0] = segmenter(student)
        writer.writerow(line) 

