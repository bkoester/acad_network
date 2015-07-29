"""Tests the simpson diversity module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import unittest

import numpy

import simpson_diversity_index


class SimpsonDiversityTest(unittest.TestCase):
    """Tests simpson_diversity_index_index module."""

    def test_calculate_unweighted_index(self):
        """Tests calculate_unweighted_index."""

        test1 = [
                numpy.array([0.1, 0.2, 0.3]),
                numpy.array([1.0, 1.5, 1.2]),
                numpy.array([]),
                numpy.array([1.0])
        ]
        expected1 = 0.387755102
        actual1 = simpson_diversity_index.calculate_unweighted_index(test1, 7)
        self.assertAlmostEqual(actual1, expected1)

        test2 = []
        expected2 = 0
        actual2 = simpson_diversity_index.calculate_unweighted_index(test2, 0)
        self.assertAlmostEqual(actual2, expected2)

        test3 = [
                numpy.array([0.1, 0.2, 0.3, 0.4, 0.5]),
                numpy.array([]),
        ]
        expected3 = 1.
        actual3 = simpson_diversity_index.calculate_unweighted_index(test3, 5)
        self.assertAlmostEqual(actual3, expected3)

    def test_calculate_weighted_index(self):
        """Tests calculate_weighted_index."""

        test1 = [
                numpy.array([0.1, 0.2, 0.3]),
                numpy.array([1.0, 1.5, 1.2]),
                numpy.array([]),
                numpy.array([1.0])
        ]
        expected1 = 0.5357778569
        actual1 = simpson_diversity_index.calculate_weighted_index(test1, 5.3)
        self.assertAlmostEqual(actual1, expected1)

        test2 = []
        expected2 = 0
        actual2 = simpson_diversity_index.calculate_weighted_index(test2, 0.)
        self.assertAlmostEqual(actual2, expected2)

        test3 = [
                numpy.array([0.1, 0.2, 0.3, 0.4, 0.5]),
                numpy.array([]),
        ]
        expected3 = 1.
        actual3 = simpson_diversity_index.calculate_weighted_index(test3, 1.5)
        self.assertAlmostEqual(actual3, expected3)


if __name__ == '__main__':
    unittest.main()
