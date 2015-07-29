"""Analyzes edge data


I.e. data output in a file format such as:

vertex1\tvertex2\tvalue
vertex1\tvertex3\tvalue
...
"""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import csv
import collections

import numpy


def get_vertices_values(edge_file):
    """Reads vertex pairs and their associated values into tuples.

    Args:
        edge_file (file-like object): File containing edges

    Returns:
        A dict of containing set{vertex1, vertex2} => value
    """
    csv_reader = csv.reader(edge_file, delimiter='\t')

    # loop through all the edges
    for vertex1, vertex2, value in csv_reader:
        yield vertex1, vertex2, float(value)


def get_vertex_total_weight(edge_lines, vertex):
    """Gets the total edge weight outgoing from any vertex.

    Args:
        edge_lines (list of vertex, vertex, value): Lines from an edge file.

    Returns:
        The total outgoing weight from a vertex.
    """
    total_weight = 0
    for vertex1, vertex2, value in edge_lines:
        if vertex1 == vertex or vertex2 == vertex:
            total_weight += value

    return total_weight

def reduce_to_vertices(edge_lines):
    """Collects all values associated with each vertex.

    Args:
        edge_lines (iterable): Tuples in the form vertex_1\tvertex_2\tvalue

    Returns:
        Dict of vertex => numpy array of values associated with vertex
    """
    vertex_values = collections.defaultdict(lambda: numpy.ndarray(0))

    for vertex1, vertex2, value in edge_lines:
        vertex_values[vertex1] = numpy.append(vertex_values[vertex1], value)
        if vertex1 != vertex2:
            vertex_values[vertex2] = numpy.append(vertex_values[vertex2], value)

    return vertex_values
