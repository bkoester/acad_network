"""Test calculation module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import calculation
import unittest


class CalculationTest(unittest.TestCase):

    def test_relative_percent_difference(self):
        test1_val1 = 1
        test1_val2 = 1
        expected1 = 0
        actual1 = calculation.relative_percent_difference(
                test1_val1, test1_val2)
        self.assertAlmostEqual(actual1, expected1)

        test2_val1 = 3
        test2_val2 = 5
        expected2 = 0.5
        actual2 = calculation.relative_percent_difference(
                test2_val1, test2_val2)
        self.assertAlmostEqual(actual2, expected2)

        test3_val1 = 0 
        test3_val2 = 1
        expected3 = 2
        actual3 = calculation.relative_percent_difference(
                test3_val1, test3_val2)
        self.assertAlmostEqual(actual3, expected3)

        test4_val1 = 1 
        test4_val2 = 0
        expected4 = 2
        actual4 = calculation.relative_percent_difference(
                test4_val1, test4_val2)
        self.assertAlmostEqual(actual4, expected4)


if __name__ == '__main__':
    unittest.main()
