"""Collects statistics from list output for each student."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import sys

import numpy

from readable_directory import ReadableDirectory
import vertex_analysis


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Analyzes a list output for '
            'each student.')
    parser.add_argument('file', nargs='?', type=argparse.FileType('r'),
            default=sys.stdin, help='File to segment (default standard input).')
    parser.add_argument('--student-archive-path', dest='student_archive_path',
            help='Path of the student archive file.', required=True)
    parser.add_argument('--swig-module-path', dest='swig_module_path',
            help='Directory containing the swig modules.',
            action=ReadableDirectory, required=True)

    args = parser.parse_args()

    vertex_lines = vertex_analysis.get_id_value_list(args.file)
    ids = (line[0] for line in vertex_lines)
    means = (mean[1] for mean in 
            vertex_analysis.accumulate_lines(vertex_lines, numpy.mean))
    medians = (median[1] for median in 
            vertex_analysis.accumulate_lines(vertex_lines, numpy.median))
    maximums = (maximum[1] for maximum in 
            vertex_analysis.accumulate_lines(vertex_lines, max))

    csv_writer = csv.writer(sys.stdout, delimiter='\t')
    for line in zip(ids, means, medians, maximums):
        csv_writer.writerow(line)
