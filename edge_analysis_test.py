"""Test edge_analysis module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import edge_analysis
import io
import unittest


class EdgeAnalysisTest(unittest.TestCase):

    def test_parse_edge_file(self):
        test1 = io.StringIO('1\t2\t0.1\n2\t3\t0.2\n1\t3\t0.3')
        expected1 = [('1', '2', '0.1'), ('2', '3', '0.2'), ('1', '3', '0.3')]
        actual1 = list(edge_analysis.parse_edge_file(test1))
        self.assertEqual(actual1, expected1)

        test2 = io.StringIO('')
        expected2 = []
        actual2 = list(edge_analysis.parse_edge_file(test1))
        self.assertEqual(actual2, expected2)

    def test_get_vertex_total_weight(self):
        test1 = [('1', '2', '0.1'), ('2', '3', '0.2'), ('1', '3', '0.3')]
        expected1 = 0.4
        actual1 = edge_analysis.get_vertex_total_weight(test1, '1')
        self.assertAlmostEqual(actual1, expected1)

        test2 = [('1', '2', '0.1'), ('2', '3', '0.2'), ('1', '3', '0.3')]
        expected2 = 0.3
        actual2 = edge_analysis.get_vertex_total_weight(test2, '2')
        self.assertAlmostEqual(actual2, expected2)

        test3 = [('1', '2', '0.1'), ('2', '3', '0.2'), ('1', '3', '0.3')]
        expected3 = 0.5
        actual3 = edge_analysis.get_vertex_total_weight(test3, '3')
        self.assertAlmostEqual(actual3, expected3)





if __name__ == '__main__':
    unittest.main()
