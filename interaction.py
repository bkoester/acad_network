"""Performs a basic reading of data from a tsv."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import collections
import csv
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

        segment_counts = collections.defaultdict(int)
        for value in tab_reader.read_column(student_file, field_index):
            if args.field in field_value_map:
                value_map = field_value_map[args.field]
                segment_counts[value_map[value]] += 1
            else:
                segment_counts[value] += 1

    # compare actual edges with expected edges
    for vertices_pair, actual_weight in actual_edges.items():
        vertex1 = vertices_pair[0]
        vertex2 = vertices_pair[1]
        vertex1_weight = segment_counts[vertex1]
        vertex2_weight = segment_counts[vertex2]
        # compute expected edge:
        if args.weightedness == 'unweighted':
            vertex1_expected_weight = (vertex2_weight / 
                    sum(segment_counts.values()) *
                    get_vertex_total_weight(actual_edges, vertices_pair[0]))
            vertex2_expected_weight = (vertex1_weight / 
                    sum(segment_counts.values()) *
                    get_vertex_total_weight(actual_edges, vertices_pair[1]))

        if args.weightedness == 'weighted':
            vertex1_expected_weight = (vertex2_weight / 
                    sum(segment_counts.values()) *
                    get_vertex_total_weight(actual_edges, vertices_pair[0]))
            vertex2_expected_weight = (vertex1_weight / 
                    sum(segment_counts.values()) *
                    get_vertex_total_weight(actual_edges, vertices_pair[1]))

            
        print('%s => %s\t%d' % (vertex1, vertex2,
            actual_weight - vertex1_expected_weight))
        if vertex1 != vertex2:
            print('%s => %s\t%d' % (vertex2, vertex1,
                actual_weight - vertex2_expected_weight))
