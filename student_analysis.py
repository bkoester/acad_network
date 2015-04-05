"""Performs basic analysis of the students tab file."""

__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import collections
import csv


import data_maps
import tab_reader


field_value_map = {
    'ethnicity': data_maps.ethnicity_codes,
    'major1': data_maps.major_codes,
}


def get_segment_counts(student_file, field):
    """Gets the number of students in various segments.

        Args:
            student_file (file-like): The file containing the student data.
            field (string): The field to get segment counts for

        Returns:
            A dict of segment => count.

        Throws:
            KeyError if the heading wasn't found in the file.
    """
    reader = csv.reader(student_file, delimiter='\t')
    headings_line = next(reader)

    heading_found = False
    for index, heading in enumerate(headings_line):
        if field.lower() in heading.lower():
            field_index = index
            heading_found = True
            break

    if not heading_found:
        raise KeyError('Could not find field "%s"!' % field)
        

    # find frequencies of the various segments in all students
    segment_counts = collections.defaultdict(int)
    for value in tab_reader.read_column(student_file, field_index):
        if field in field_value_map:
            value_map = field_value_map[field]
            segment_counts[value_map[value]] += 1
        else:
            segment_counts[value] += 1

    return segment_counts
