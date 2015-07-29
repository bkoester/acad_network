"""Test edge_analysis module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import edge_analysis
import io
import unittest

import numpy


class EdgeAnalysisTest(unittest.TestCase):
    """Tests edge_analysis module."""

    def _test_dict_to_numpy_array_equal(self, actual, expected):
        """Assert-style method comparing dicts of <key type> => numpy array."""

        self.assertEqual(len(actual.keys()), len(expected.keys()))
        for key, expected_np_array in expected.items():
            self.assertIn(key, actual)
            actual_np_array = actual[key]
            numpy.testing.assert_array_equal(actual_np_array, expected_np_array)

    def test_get_vertices_values(self):
        """Tests get_vertices_values."""

        test1 = io.StringIO('1\t2\t0.1\n2\t3\t0.2\n1\t3\t0.3')
        expected1 = [('1', '2', 0.1), ('2', '3', 0.2), ('1', '3', 0.3)]
        actual1 = list(edge_analysis.get_vertices_values(test1))
        self.assertEqual(actual1, expected1)

        test2 = io.StringIO('')
        expected2 = []
        actual2 = list(edge_analysis.get_vertices_values(test2))
        self.assertEqual(actual2, expected2)

    def test_get_vertex_total_weight(self):
        """Tests get_vertex_total_weight."""

        test1 = [('1', '2', 0.1), ('2', '3', 0.2), ('1', '3', 0.3)]
        expected1 = 0.4
        actual1 = edge_analysis.get_vertex_total_weight(test1, '1')
        self.assertAlmostEqual(actual1, expected1)

        test2 = [('1', '2', 0.1), ('2', '3', 0.2), ('1', '3', 0.3)]
        expected2 = 0.3
        actual2 = edge_analysis.get_vertex_total_weight(test2, '2')
        self.assertAlmostEqual(actual2, expected2)

        test3 = [('1', '2', 0.1), ('2', '3', 0.2), ('1', '3', 0.3)]
        expected3 = 0.5
        actual3 = edge_analysis.get_vertex_total_weight(test3, '3')
        self.assertAlmostEqual(actual3, expected3)

    def test_reduce_to_vertices(self):
        """Tests reduce_to_vertices."""

        test1 = [('1', '2', 0.1), ('2', '3', 0.2), ('1', '3', 0.3)]
        expected1 = {
                '1': numpy.array([0.1, 0.3]),
                '2': numpy.array([0.1, 0.2]),
                '3': numpy.array([0.2, 0.3]),
        }
        actual1 = edge_analysis.reduce_to_vertices(test1)
        self._test_dict_to_numpy_array_equal(actual1, expected1)

        test2 = []
        expected2 = {}
        actual2 = edge_analysis.reduce_to_vertices(test2)
        self._test_dict_to_numpy_array_equal(actual2, expected2)

        test3 = [('1', '1', 0.1), ('1', '1', 0.2), ('1', '1', 0.3)]
        expected3 = {
                '1': numpy.array([0.1, 0.2, 0.3])
        }
        actual3 = edge_analysis.reduce_to_vertices(test3)
        self._test_dict_to_numpy_array_equal(actual3, expected3)


if __name__ == '__main__':
    unittest.main()
