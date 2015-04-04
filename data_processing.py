"""Performs a basic processing of data."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import collections
import csv
import numpy


def segment_data(data_file, segment=True):
    """Segments input data.

    Args:
        data_file (file-like object): File from which to read data
        segment (bool): Whether to segment the data or not

    Returns:
        A dict of containing the segment => list
    """
    # read in the csv file of degrees from the file
    segments = collections.defaultdict(lambda: numpy.ndarray(0))
    for line in csv.reader(data_file, delimiter='\t'):
        # add the datum to the correct segment
        segment = line[0] if segment else 'all'
        datum = line[1]
        segments[segment] = numpy.append(segments[segment], float(datum))

    return segments


def get_edges(data_file):
    """Reads vertex pairs and their associated values into a dict.

    Args:
        data_file (file-like object): File containing edges

    Returns:
        A dict of containing set{vertex1, vertex2} => value
    """
    edges = dict()

    # skip the heading line of the csv file
    csv_reader = csv.reader(data_file, delimiter='\t')
    next(csv_reader)

    # loop through all the edges
    for line in csv_reader:
        vertex1 = line[0]
        vertex2 = line[1]
        value = float(line[2])

        # store the edges in a dict
        edges[(vertex1, vertex2)] = value

    return edges


def percentages(frequencies):
    """Changes frequencies into percentages for a given iterable.

        Args:
            frequencies (iterable): A iterable of numeric frequencies to change
                into percentages.

        Returns:
            A list containing corresponding percentages of frequencies.
    """
    return [frequency / sum(data) for frequency in frequencies]
