"""Calculates simpson's diversity index for an individual.

Expects input as a file or files in the form:

vertex1\tvalue
vertex2\tvalue
...

where vertexn is another student the individual is connected to.
"""

__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import sys

import numpy

from readable_directory import ReadableDirectory
from student_container_wrapper import StudentContainerWrapper
import vertex_analysis


def calculate_unweighted_index(segment_values, total_connections):
    """Calculats a standard unweighted simpson diversity index for a student.

    Args:
        values (iterable of numpy array): Numpy arrays whose length will be
            used to determine proportions (i.e.values in arrays are ignored).
        total_connections (int): The total number of connections the student
            has.

    Returns:
        A float representing the student's unweighted simpson diversity index.
    """
    diversity_index = 0
    for values in segment_values:
        # use only the number of values in the segment if unweighted
        diversity_index += (values.size / total_connections) ** 2

    return diversity_index


def calculate_weighted_index(segment_values, values_sum):
    """Calculats a weighted simpson diversity index for a student.

    Args:
        values (iterable of numpy array): Numpy arrays containing weights to be
            used when calculating proportions.
        values_sum (float): The sum of the weights of all the connections.

    Returns:
        A float representing the student's unweighted simpson diversity index.
    """
    diversity_index = 0
    for values in segment_values:
        diversity_index += (numpy.sum(values) / values_sum) ** 2

    return diversity_index



if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Calculate diversity index for an individual given '
            'their connections.')
    parser.add_argument(
            '-s', '--segmenter', default='all',
            choices=StudentContainerWrapper.SEGMENTERS.keys(),
            help='Segmenting function to use.')
    parser.add_argument(
            'weightedness', choices=['weighted', 'unweighted'],
            help='Calculate using weights of connections or just by frequency '
            'of connections.')
    parser.add_argument(
            '-f', '--files', nargs='*', type=argparse.FileType('r'),
            default=sys.stdin, help='Files to parse (default standard input).')
    parser.add_argument(
            '--student-archive-path', dest='student_archive_path',
            help='Path of the student archive file.', required=True)
    parser.add_argument(
            '--swig-module-path', dest='swig_module_path',
            help='Directory containing the swig modules.',
            action=ReadableDirectory, required=True)

    args = parser.parse_args()

    # segment the network for analysis
    students_wrapper = StudentContainerWrapper(
            args.swig_module_path, args.student_archive_path)
    segmenter = StudentContainerWrapper.SEGMENTERS[args.segmenter]

    for vertex_file in args.files:
        vertex_lines = vertex_analysis.get_id_values(vertex_file)

        segmented_lines = list(vertex_analysis.map_to_segments(
                vertex_lines, segmenter, students_wrapper))
        total_connections = len(segmented_lines)
        values_sum = vertex_analysis.accumulate(segmented_lines, sum)
        segments = vertex_analysis.reduce_to_segments(segmented_lines)

        # calculate the diversity of the segments
        diversity_index = 0
        if args.weightedness == 'unweighted':
            diversity_index = calculate_unweighted_index(
                    segments.values(), total_connections)
        elif args.weightedness == 'weighted':
            diversity_index = calculate_weighted_index(
                    segments.values(), values_sum)
        else:
            raise ValueError('Unknown argument for weightedness given!')

        if len(args.files) > 1:
            print('{}\t{}'.format(vertex_file.name, diversity_index))
        else:
            print(diversity_index)
