"""Performs a basic reading of data from a tsv."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import csv


#class TabReader(csv.reader):
#
#    def __init__(file_obj, *args, **kwargs):
#        super.__init__(data_file, *args, delimiter='\t', **kwargs)
#

def read_column(data_file, column_num):
    """Segments input data.

    Args:
        data_file (file-like object): File from which to read data.
        column_num (int): The column number to read.

    Yields:
        The column values.
    """
    for line in csv.reader(data_file, delimiter='\t'):
        yield line[column_num]
