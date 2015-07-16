"""Performs calculations not built in to python or libraries."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


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
    rpd = abs(2 * difference / value_sum)
    return rpd

