"""Performs a basic processing of data."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import collections
import csv
import numpy


def segment_data(data_file, segment):
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
