"""Performs a basic reading of data from a tsv."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import collections
import csv
import matplotlib.pyplot
import numpy
import sys

import data_processing
import tab_reader


ethnicity_codes = {
    '0': "Unknown",
    '1': "Hispanic only",
    '2': "American Indian or Alaska Native",
    '3': "Asian only",
    '4': "Black or African American",
    '5': "Hawaiian or other Pacific Islander",
    '6': "White only",
    '7': "Multiracial",
    '8': "Unknown",
    '9': "Nonresident alien"
}

field_value_map = {
    'ethnicity': ethnicity_codes,
}


def get_vertex_total_weight(edges, vertex):
    """Gets the total edge weight outgoing from any vertex.

    Args:
        edges (pair => int): The map of edges.

    Returns:
        The total outgoing weight from a vertex
    """
    total_weight = 0
    for vertices, weight in edges.items():
        if vertices[0] == vertex or vertices[1] == vertex:
            total_weight += weight

    return total_weight


def relative_percent_difference(value1, value2):
    """Calculates relative percent difference between two values.
    
    Args:
        value1 (number)
        value2 (number)

    Returns:
        The relative percent difference between the two values.
    """
    difference = value1 - value2
    value_sum = value1 + value2
    rpd = 2 * difference / value_sum
    return rpd


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Analyze data dealing with'
        'networks.')
    parser.add_argument('-f', '--student-file', dest='student_path', 
            default='../data/student.tab',
            help='The file containing edge interactions.')
    parser.add_argument('field', 
        help='The field for which to analyze interactions.')
    parser.add_argument('weightedness', 
        help='Either "weighted" or "unweighted"')
        
    args = parser.parse_args()

    if args.weightedness not in ['weighted', 'unweighted']:
        print('Weightedness %s not known!' % args.weightedness)
        exit()

    # get actual interaction values
    actual_edges = data_processing.get_edges(sys.stdin)

    # get expected interaction values
    with open(args.student_path, 'r') as student_file:
        reader = csv.reader(student_file, delimiter='\t')
        headings_line = next(reader)

        heading_found = False
        for index, heading in enumerate(headings_line):
            if args.field.lower() in heading.lower():
                field_index = index
                heading_found = True

        if not heading_found:
            print('Could not find field "%s"!' % args.field)
            exit()

        # find frequencies of the various segments in all students
        segment_counts = collections.defaultdict(int)
        for value in tab_reader.read_column(student_file, field_index):
            if args.field in field_value_map:
                value_map = field_value_map[args.field]
                segment_counts[value_map[value]] += 1
            else:
                segment_counts[value] += 1

    # compare actual edges with expected edges, put into matrix
    segments = sorted(list(segment_counts.keys()))
    population = sum(segment_counts.values())
    heatmatrix = numpy.zeros((len(segments), len(segments)))
    for vertices_pair, actual_weight in actual_edges.items():
        vertex1 = vertices_pair[0]
        vertex2 = vertices_pair[1]
        vertex1_weight = segment_counts[vertex1]
        vertex2_weight = segment_counts[vertex2]

        # store vertex expected weight
        v1v2_expected_weight = ((vertex2_weight / population) *
                get_vertex_total_weight(actual_edges, vertex1))
        v2v1_expected_weight = ((vertex1_weight / population) *
                get_vertex_total_weight(actual_edges, vertex2))

        # store the difference of actual and expected in a matrix
        vertex1_index = segments.index(vertex1)
        vertex2_index = segments.index(vertex2)
        heatmatrix[vertex1_index, vertex2_index] = relative_percent_difference(
                actual_weight, v1v2_expected_weight)
        if vertex1 != vertex2:
            heatmatrix[vertex2_index, vertex1_index] = (
                    relative_percent_difference(
                        actual_weight, v2v1_expected_weight))
            

    fig, ax = matplotlib.pyplot.subplots()
    heatmap = matplotlib.pyplot.pcolor(
            heatmatrix, cmap=matplotlib.pyplot.cm.RdBu)
    heatmap.set_clim(vmin=-1.0, vmax=1.0)

    # where the ticks are located
    ax.invert_yaxis()
    ax.xaxis.tick_top()

    # tick locations relative to the cell
    ax.set_xticks(numpy.arange(heatmatrix.shape[0]) + 0.5, minor=False)
    ax.set_yticks(numpy.arange(heatmatrix.shape[1]) + 0.5, minor=False)
    # stop a white bar from appearing due to rounding
    ax.set_xlim(right=len(segments))
    ax.set_ylim(bottom=len(segments))

    # tick labels
    ax.set_xticklabels(segments, minor=False)
    ax.set_yticklabels(segments, minor=False)

    # rotation of x-axis
    matplotlib.pyplot.xticks(rotation=90)

    # labels
    matplotlib.pyplot.xlabel('%s interactions from ...' %
            (args.weightedness.title()))
    ax.xaxis.set_label_position('top')
    matplotlib.pyplot.ylabel('To ...')

    matplotlib.pyplot.colorbar(heatmap)
    fig.tight_layout()
    matplotlib.pyplot.show()
