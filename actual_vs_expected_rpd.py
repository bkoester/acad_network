"""Compares RPD of actual vs. expected interactions for a given student.

Expects a list of student IDs to which the analyzed student ID is connected and
corresponding edge values, i.e.

student_id_1\tvalue_11
student_id_2\tvalue_21
...

"""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import sys

import calculation
import readable_directory
import segment
import vertex_analysis
from student_container_wrapper import StudentContainerWrapper


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Analyze interactions of an individual student')
    parser.add_argument(
            'field', help='The field for which to analyze interactions.')
    parser.add_argument(
            'weightedness', choices=['weighted', 'unweighted'],
            help='Either "weighted" or "unweighted".')
    parser.add_argument(
            'in_segment', help='To which segment the input student belongs to.')
    parser.add_argument(
            'file', nargs='?', type=argparse.FileType('r'),
            default=sys.stdin, help='File to segment (default standard input).')
    parser.add_argument(
            '--student-archive-path', dest='student_archive_path',
            help='Path of the student archive file.', required=True)
    parser.add_argument(
            '--swig-module-path', dest='swig_module_path',
            help='Directory containing the swig modules.',
            action=readable_directory.ReadableDirectory, required=True)

    args = parser.parse_args()

    unweighted = args.weightedness == 'unweighted'
    weighted = args.weightedness == 'weighted'

    # get container of students
    students = StudentContainerWrapper(
            args.swig_module_path, args.student_archive_path)

    # get the weights of individual segments
    lines = list(vertex_analysis.get_id_values(args.file))
    mapped_to_segments = vertex_analysis.map_to_segments(
            lines, StudentContainerWrapper.SEGMENTERS[args.field], students)
    reduced_data = vertex_analysis.reduce_to_in_out(
            mapped_to_segments, args.in_segment)
    if unweighted:
        actual_segments = {key: len(data) for key, data in reduced_data.items()}
    elif weighted:
        actual_segments = {key: sum(data) for key, data in reduced_data.items()}

    print(actual_segments)

    population_segments = segment.get_segment_counts(students, args.field)
    print(population_segments[args.in_segment])

    population = len(students)
    total_weight = sum(line[1] for line in lines) if weighted else len(lines)

    # print actual weight vs. expected weight RPD
    expected_weight = (population_segments[args.in_segment] / population *
                      total_weight)
    actual_weight = actual_segments[args.in_segment]

    print(calculation.relative_percent_difference(
            actual_weight, expected_weight))
