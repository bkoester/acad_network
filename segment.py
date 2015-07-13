"""Utility to segment students."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import sys

from course_container_wrapper import CourseContainerWrapper
import readable_directory
from student_container_wrapper import StudentContainerWrapper


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


def reduce_segment_to_in_out(segments, in_segment):
    """Reduces segment categories to an "in" category and an "out" category.

        Args:
            segments (string => num): Current dict of segments.
            in_segment (string): The in segment to maintain.

        Returns:
            string => num with keys in_segment and "out" and corresponding
                weights
    """
    reduced_segments = {
        in_segment: segments[in_segment],
        'out': 0
    }

    for segment, weight in segments.items():
        if segment == in_segment:
            continue
        reduced_segments['out'] += weight

    return reduced_segments


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

    # segment IDs (use the first field in the line as the ID)
    segmenter = StudentContainerWrapper.SEGMENTERS[args.field]
    writer = csv.writer(sys.stdout, delimiter=args.delimiter)
    for line in csv.reader(args.file, delimiter=args.delimiter):
        # replace the student ID with the segment value
        student_id = int(line[0])
        line[0] = students_wrapper.segment_student(student_id, segmenter)
        writer.writerow(line) 

