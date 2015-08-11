"""Compares RPD of actual vs. expected interactions of nodes in a network.

Expects a list of edges in the network, i.e.

vertex1\tvertex2\tvalue
vertex1\tvertex3\tvalue
...

"""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import math
import sys

import matplotlib.pyplot
import numpy

import calculation
import edge_analysis
from readable_directory import ReadableDirectory
from student_container_wrapper import StudentContainerWrapper
import segment


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Show RPD between actual and expected interactions of '
            'segments.')
    parser.add_argument(
            'field', help='The field for which to analyze interactions.')
    parser.add_argument(
            'weightedness', choices=['weighted', 'unweighted'],
            help='Either "weighted" or "unweighted".')
    parser.add_argument(
            'file', nargs='?', type=argparse.FileType('r'),
            default=sys.stdin, help='File to segment (default standard input).')
    parser.add_argument(
            '--student-archive-path', dest='student_archive_path',
            help='Path of the student archive file.', required=True)
    parser.add_argument(
            '--swig-module-path', dest='swig_module_path',
            help='Directory containing the swig modules.',
            action=ReadableDirectory, required=True)

    args = parser.parse_args()

    # read edges from file; these correspond to actual interaction values
    edge_lines = list(edge_analysis.get_vertices_values(args.file))

    # get expected interaction values
    students_wrapper = StudentContainerWrapper(
            args.swig_module_path, args.student_archive_path)
    segment_counts = segment.get_segment_counts(students_wrapper, args.field)

    # compare actual edges with expected edges, put into matrix
    segments = sorted(list(segment_counts.keys()))
    population = sum(segment_counts.values())
    heatmatrix = numpy.zeros((len(segments), len(segments)))
    for vertex1, vertex2, actual_weight in edge_lines:
        vertex1_weight = segment_counts[vertex1]
        vertex2_weight = segment_counts[vertex2]

        # store vertex expected weight
        v1v2_expected_weight = (
                (vertex2_weight / population) *
                edge_analysis.get_vertex_total_weight(edge_lines, vertex1))
        v2v1_expected_weight = (
                (vertex1_weight / population) *
                edge_analysis.get_vertex_total_weight(edge_lines, vertex2))

        # store the difference of actual and expected in a matrix
        vertex1_index = segments.index(vertex1)
        vertex2_index = segments.index(vertex2)
        heatmatrix[vertex2_index, vertex1_index] = (
                calculation.relative_percent_difference(
                        actual_weight, v1v2_expected_weight))

        if vertex1 != vertex2:
            heatmatrix[vertex1_index, vertex2_index] = (
                    calculation.relative_percent_difference(
                            actual_weight, v2v1_expected_weight))

    if len(segments) > 50:
        # print out just segment RPDs with themselves
        segment_self_rpds = dict()
        for i in range(0, heatmatrix.shape[0]):
            segment_self_rpds[segments[i]] = heatmatrix[i, i]

        # sort them
        sorted_segment_self_rpds = sorted(
                [(segment, self_rpd) for (segment, self_rpd)
                 in segment_self_rpds.items()],
                key=lambda x: x[1], reverse=True)

        for segment, self_rpd in sorted_segment_self_rpds:
            print('{}\t{}'.format(segment, self_rpd))

    else:
        figsize = (3 + math.ceil(len(segments) * 0.6),
                   2 + math.ceil(len(segments) * 0.4))
        fig = matplotlib.pyplot.figure(figsize=figsize)
        ax = fig.add_subplot(1, 1, 1)
        heatmap = matplotlib.pyplot.pcolor(heatmatrix, cmap='RdBu')
        heatmap.set_clim(vmin=-1.0, vmax=1.0)

        # where the ticks are located
        ax.invert_yaxis()
        ax.xaxis.tick_top()

        # tick locations relative to the cell
        ax.set_xticks(numpy.arange(heatmatrix.shape[0]) + 0.5, minor=False)
        ax.set_yticks(numpy.arange(heatmatrix.shape[1]) + 0.5, minor=False)
        matplotlib.pyplot.tick_params(
                axis='x', which='both', bottom='off', top='off')
        matplotlib.pyplot.tick_params(
                axis='y', which='both', right='off', left='off')

        # stop a white bar from appearing due to rounding
        ax.set_xlim(right=len(segments))
        ax.set_ylim(bottom=len(segments))

        # tick labels
        ax.set_xticklabels(segments, minor=False)
        ax.set_yticklabels(segments, minor=False)

        # rotation of x-axis
        matplotlib.pyplot.xticks(rotation=90)

        # labels
        matplotlib.pyplot.xlabel(
                '{} interactions from ...'.format(args.weightedness.title()))
        ax.xaxis.set_label_position('top')
        matplotlib.pyplot.ylabel('To ...')

        matplotlib.pyplot.colorbar(heatmap)

        # put the numbers in the boxes
        for y in range(heatmatrix.shape[0]):
            for x in range(heatmatrix.shape[1]):
                color = 'black' if abs(heatmatrix[y, x]) < 0.5 else 'white'
                matplotlib.pyplot.text(
                        x + 0.5, y + 0.5, '%.2f' % heatmatrix[y, x],
                        horizontalalignment='center',
                        verticalalignment='center',
                        color=color)

        fig.tight_layout()
        matplotlib.pyplot.show()
