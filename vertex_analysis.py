"""Analyzes vertex data


I.e. data output in a file format such as:

vertex1\tvalue
vertex2\tvalue
...
"""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import collections
import csv
import numpy


def parse_vertex_file(vertex_file):
    """Reads the data file into an iterable.

    Args:
        vertex_file (file-like object): The file to parse in the format given
            above.

    Yields:
        One line of the file at a time.
    """
    csv_reader = csv.reader(vertex_file, delimiter='\t')
    for row in csv_reader:
        yield tuple(row)


def map_to_segments(vertex_file_lines, segmenter, students_wrapper):
    """Maps student IDs to segments.

        Args:
            vertex_file_lines (list): ID, value pairs
            segmenter (function): Takes a student ID and maps it to a segment.
            students_wrapper (StudentContainerWrapper): Students container.

        Yields:
            A line at a time containing segment, value
    """

    for student_id_str, value in vertex_file_lines:
        student_id = int(student_id_str)
        yield students_wrapper.segment_student(student_id, segmenter), value


def reduce_to_segments(segmented_data):
    """Reduces input data into segments.

    Args:
        segmented_data (iterable): Iterable from which to read segments

    Returns:
        A dict of the segment => numpy array
    """
    # read in the csv file of degrees from the file
    segments = collections.defaultdict(lambda: numpy.ndarray(0))
    for segment, value in segmented_data:
        segments[segment] = numpy.append(segments[segment], float(value))

    return segments


def reduce_to_all(segmented_data):
    """Reduces all data into a single category

    Args:
        segmented_data (iterable): Iterable from which to read segments

    Returns:
        A dict of 'all' => numpy array
    """
    # read in the csv file of degrees from the file
    values = numpy.ndarray(0)
    for _, value in segmented_data:
        values = numpy.append(values, float(value))

    return { 'all': values }

def reduce_to_in_out(segmented_data, in_segment):
    """Reduces data into "in" segment and "out" segment

    Args:
        segmented_data (iterable): Iterable from which to read segments
        in_segment (string): The segment that the "in" group

    Returns:
        A dict of 'all' => numpy array
    """

    in_segment_values = numpy.array([])
    out_segment_values = numpy.array([])

    for segment, value in segmented_data:
        if segment == in_segment:
            in_segment_values = numpy.append(in_segment_values, float(value))
        else:
            out_segment_values = numpy.append(out_segment_values, float(value))

    return { 
        in_segment: in_segment_values,
        ' '.join(['not', in_segment]): out_segment_values,
    }
