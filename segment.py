"""Utility to segment students.

Expects input as a file in the form:

vertex1\tvalue
vertex2\tvalue
...

where vertexn a student ID
"""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import sys

from course_container_wrapper import CourseContainerWrapper
import readable_directory
from student_container_wrapper import StudentContainerWrapper
import vertex_analysis


def get_segment_counts(students, field):
    """Gets the number of students in various segments.

        Args:
            students (StudentContainerWrapper): The wrapper containing the
                students
            field (string): The field to get segment counts for

        Returns:
            A dict of segment => count.

        Throws:
            KeyError if the heading wasn't found in the file.
    """
    segmenter = StudentContainerWrapper.SEGMENTERS[field]

    # find frequencies of the various segments in all students
    segment_counts = collections.defaultdict(int)
    for student in students:
        segment_counts[segmenter(student)] += 1
        
    return segment_counts


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
        help='Directory containing the swig modules.',
        action=readable_directory.ReadableDirectory, required=True)

    args = parser.parse_args()

    students_wrapper = StudentContainerWrapper(
            args.swig_module_path, args.student_archive_path)
    courses_wrapper = CourseContainerWrapper(
            args.swig_module_path, args.course_archive_path)
    students_wrapper.load_courses(courses_wrapper.courses)

    # segment students
    segmenter = StudentContainerWrapper.SEGMENTERS[args.field]
    vertex_lines = vertex_analysis.get_id_values(args.file) 
    segment_mapped_lines = vertex_analysis.map_to_segments(
            vertex_lines, segmenter, students_wrapper)

    # write out the segmented students
    writer = csv.writer(sys.stdout, delimiter=args.delimiter)
    for segment, value in segment_mapped_lines:
        writer.writerow([segment, value]) 
