"""Calculates simpson's diversity index for a network.

Expects input as a file or files in the form:

vertex_1\tvertex_1\tvalue
vertex_1\tvertex_2\tvalue
...
vertex_1\tvertex_n\tvalue
vertex_2\tvertex_2\tvalue
...
vertex_2\tvertex_n\tvalue
...
vertex_n\tvertex_n\tvalue
"""

__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import sys

import numpy

import edge_analysis
import simpson_diversity

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Calculate simpson diversity index for every vertex in '
            'a network.')
    parser.add_argument(
            'file', nargs='?', type=argparse.FileType('r'),
            default=sys.stdin, help='File to segment (default standard input).')

    args = parser.parse_args()

    edge_lines = edge_analysis.get_vertices_values(args.file)
    edge_values = edge_analysis.reduce_to_vertices(edge_lines)

    for vertex, values in edge_values.items():
        # make every value in values an array because calculation functions
        # expect multiple arrays grouped by segment
        values_sum = numpy.sum(values)
        single_value_arrays = (numpy.array(value) for value in values)

        # We use weighted regardless of the input file because we the value to
        # be taken into account. There is only one value, so looking at the
        # number of values is useless.
        diversity_index = (
                simpson_diversity.calculate_weighted_simpson_diversity_index(
                        single_value_arrays, values_sum))

        print('{}\t{}'.format(vertex, diversity_index))
