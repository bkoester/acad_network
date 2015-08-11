"""Analyzes vertex data


I.e. data output in a file format such as:

student_id_1\tvalue_11[\tvalue_12\t...\tvalue_1n]
student_id_2\tvalue_21[\tvalue_22\t...\tvalue_2n]
...

Will also accept (if student IDs aren't appropriate):

name_1\tvalue_11[\tvalue_12\t...\tvalue_1n]
name_2\tvalue_21[\tvalue_22\t...\tvalue_2n]
...

The terminology "values" is used often throughout this file. Values always
refers to the fields marked values as given above. Values can be processed in a
few different ways. They can either be combined into a list `get_id_value_list`,
or presented as a tuple `get_id_values` flat with the student ID.

It might be useful to combine values into a list if there are many values, while
if there are just a few, then presenting them flat with the tuple may make more
sense.

Essentially, this module allows processes to operate on id => values like a
key-value store.
"""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import collections
import csv

import numpy


def _are_values_flat(values):
    """Tests whether values are stored flat with the student_id.

    Args:
        values (list): List of values to test.

    Returns:
        Boolean if values are stored as flat.
    """
    return len(values) > 0 and not isinstance(values[0], list)


def _are_values_listed(values):
    """Tests whether values are stored as a list.

    Args:
        values (list): List of values to test.

    Returns:
        Boolean if values are stored as flat.
    """
    return len(values) == 1 and isinstance(values[0], list)

def _iterate_over_all_values(vertex_file_lines):
    """Iterates over all values in the set of lines.

    Args:
        vertex_file_lines (iterable): (id, *values) tuples.

    Yields:
        One value at a time.
    """
    for _, *values in vertex_file_lines:
        if _are_values_listed(values):
            for value in values[0]:
                yield value
        elif _are_values_flat(values):
            for value in values:
                yield value
        else:
            raise ValueError('Values are of unknown type (should be either flat'
                             ' with the student ID or in a list.')


def get_name_values(vertex_file):
    """Reads the data file into an iterable of (name, *values)

    Args:
        vertex_file (file-like object): The file to parse in the format given
            above.

    Yields:
        Tuple of (name, *values).
    """
    csv_reader = csv.reader(vertex_file, delimiter='\t')
    for name, *values in csv_reader:
        yield tuple([name] + [float(value) for value in values])


def get_id_values(vertex_file):
    """Reads the data file into an iterable of (id, *values)

    Args:
        vertex_file (file-like object): The file to parse in the format given
            above.

    Yields:
        Tuple of (id, *values).
    """
    csv_reader = csv.reader(vertex_file, delimiter='\t')
    for student_id, *values in csv_reader:
        yield tuple([int(student_id)] + [float(value) for value in values])


def get_id_value_list(vertex_file):
    """Reads the data file into an iterable of (id, [values])

    Args:
        vertex_file (file-like object): The file to parse in the format given
            above.

    Yields:
        One line of the file at a time.
    """
    csv_reader = csv.reader(vertex_file, delimiter='\t')
    for student_id, *values in csv_reader:
        yield int(student_id), [float(value) for value in values]


def accumulate(vertex_file_lines, func):
    """Performs a function

    Args:
        vertex_file_lines (iterable): (id, *values) tuples.
        func (function): A function that takes a list.

    Returns:
        func applied to all values.
    """
    return func(_iterate_over_all_values(vertex_file_lines))


def accumulate_lines(vertex_file_lines, func):
    """Applies a given function to all the values in the line.

    Essentially, this is a glorified version of python's built-in map function
    operating on each set of values.

    Args:
        vertex_file_lines (iterable): (id, *values) tuples .
        func (function): A function that takes a list.

    Yields:
        func(values) for each line.
    """
    for student_id, *values in vertex_file_lines:
        if _are_values_listed(values):
            yield student_id, func(values[0])
        elif _are_values_flat(values):
            yield student_id, func(values)
        else:
            raise ValueError('Values are of unknown type (should be either flat'
                             ' with the student ID or in a list.')


def map_to_segments(vertex_file_lines, segmenter, students_wrapper):
    """Maps student IDs to segments.

        Args:
            vertex_file_lines (iterable): (ID, value) pairs
            segmenter (function): Takes a student ID and maps it to a segment.
            students_wrapper (StudentContainerWrapper): Students container.

        Yields:
            A line at a time containing segment, value
    """
    for student_id, *values in vertex_file_lines:
        yield tuple([students_wrapper.segment_student(student_id, segmenter)] +
                    values)


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

    return {'all': values}

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

    return {in_segment: in_segment_values,
            'not {}'.format(in_segment): out_segment_values,
           }
