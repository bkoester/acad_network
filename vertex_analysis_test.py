"""Test vertex_analysis module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import io
import numpy
import vertex_analysis
import unittest
import unittest.mock


class VertexAnalysisTest(unittest.TestCase):

    def _test_dict_to_numpy_array_equal(self, actual, expected):
        self.assertEqual(len(actual.keys()), len(expected.keys()))
        for key, expected_np_array in expected.items():
            self.assertIn(key, actual)
            actual_np_array = actual[key]
            numpy.testing.assert_array_equal(actual_np_array, expected_np_array)

    def test_get_id_values(self):
        test1 = io.StringIO('1\t1.0\n2\t2.0\n3\t3.0')
        expected1 = [(1, 1.0), (2, 2.0), (3, 3.0)]
        actual1 = list(vertex_analysis.get_id_values(test1))
        self.assertEqual(actual1, expected1)
        
        test2 = io.StringIO('')
        expected2 = []
        actual2 = list(vertex_analysis.get_id_values(test2))
        self.assertEqual(actual2, expected2)

        test3 = io.StringIO('1\t1.0\t1.0\n2\t2.0\t2.0\n3\t3.0\t3.0')
        expected3 = [(1, 1.0, 1.0), (2, 2.0, 2.0), (3, 3.0, 3.0)]
        actual3 = list(vertex_analysis.get_id_values(test3))
        self.assertEqual(actual3, expected3)

    def test_get_id_value_list(self):
        test1 = io.StringIO('1\t1.0\n2\t2.0\n3\t3.0')
        expected1 = [(1, [1.0]), (2, [2.0]), (3, [3.0])]
        actual1 = list(vertex_analysis.get_id_value_list(test1))
        self.assertEqual(actual1, expected1)
        
        test2 = io.StringIO('')
        expected2 = []
        actual2 = list(vertex_analysis.get_id_value_list(test2))
        self.assertEqual(actual2, expected2)

        test3 = io.StringIO('1\t1.0\t1.0\n2\t2.0\t2.0\n3\t3.0\t3.0')
        expected3 = [(1, [1.0, 1.0]), (2, [2.0, 2.0]), (3, [3.0, 3.0])]
        actual3 = list(vertex_analysis.get_id_value_list(test3))
        self.assertEqual(actual3, expected3)

    def test_accumulate(self):
        test1 = [(1, 1.0), (2, 2.0), (3, 3.0)]
        expected1 = 6.0
        actual1 = vertex_analysis.accumulate(test1, sum)
        self.assertAlmostEqual(actual1, expected1)

        test2 = []
        with self.assertRaises(ValueError):
            vertex_analysis.accumulate(test2, max)

        test3 = [(1, [0.2])]
        expected3 = 0.2
        actual3 = vertex_analysis.accumulate(test3, min)
        self.assertAlmostEqual(actual3, expected3)

    def test_accumulate_lines(self):
        test1 = [(1, 1.0, 1.0), (2, 2.0, 2.0), (3, 3.0, 3.0)]
        expected1 = [(1, 2.0), (2, 4.0), (3, 6.0)]
        actual1 = list(vertex_analysis.accumulate_lines(test1, sum))
        self.assertEqual(actual1, expected1)

        test2 = []
        expected2 = [] 
        actual2 = list(vertex_analysis.accumulate_lines(test2, max))
        self.assertEqual(actual2, expected2)

        test3 = [(1, [2.0, 1.0]), (2, [2.0, 3.0, 1.0])]
        expected3 = [(1, 1.0), (2, 1.0)]
        actual3 = list(vertex_analysis.accumulate_lines(test3, min))
        self.assertEqual(actual3, expected3)

    def test_map_to_segments(self):
        segmenter = None

        test1 = [(1, 1.0), (2, 2.0), (3, 3.0)]
        expected1 = [('ULSA', 1.0), ('UENG', 2.0), ('UKIN', 3.0)]
        mock_student_wrapper1 = unittest.mock.MagicMock()
        mock_student_wrapper1.segment_student = unittest.mock.MagicMock(
                side_effect = ['ULSA', 'UENG', 'UKIN'])
        actual1 = list(vertex_analysis.map_to_segments(
            test1, segmenter, mock_student_wrapper1))

        self.assertEqual(actual1, expected1)

        test2 = []
        expected2 = []
        mock_student_wrapper2 = unittest.mock.MagicMock()
        actual2 = list(vertex_analysis.map_to_segments(
            test2, segmenter, mock_student_wrapper2))
        self.assertEqual(actual2, expected2)

    def test_reduce_to_segments(self):
        test1 = [('ULSA', 1.0), ('UENG', 2.0), ('UKIN', 3.0), ('ULSA', 1.0)]
        expected1 = {
                'ULSA': numpy.array([1.0, 1.0]),
                'UENG': numpy.array([2.0]),
                'UKIN': numpy.array([3.0])
        }
        actual1 = vertex_analysis.reduce_to_segments(test1)
        self._test_dict_to_numpy_array_equal(actual1, expected1)

        test2 = []
        expected2 = dict()
        actual2 = vertex_analysis.reduce_to_segments(test2)
        self._test_dict_to_numpy_array_equal(actual2, expected2)

        test3 = [('ULSA', 1.0), ('ULSA', 2.0), ('ULSA', 3.0), ('ULSA', 1.0)]
        expected3 = {
                'ULSA': numpy.array([1.0, 2.0, 3.0, 1.0]),
        }
        actual3 = vertex_analysis.reduce_to_segments(test3)
        self._test_dict_to_numpy_array_equal(actual3, expected3)

    def test_reduce_to_all(self):
        test1 = [('ULSA', 1.0), ('UENG', 2.0), ('UKIN', 3.0), ('ULSA', 1.0)]
        expected1 = { 'all': numpy.array([1.0, 2.0, 3.0, 1.0]), }
        actual1 = vertex_analysis.reduce_to_all(test1)
        self._test_dict_to_numpy_array_equal(actual1, expected1)

        test2 = []
        expected2 = { 'all': numpy.array([]), }
        actual2 = vertex_analysis.reduce_to_all(test2)
        self._test_dict_to_numpy_array_equal(actual2, expected2)

        test3 = [('ULSA', 1.0), ('ULSA', 2.0), ('ULSA', 3.0), ('ULSA', 1.0)]
        expected3 = { 'all': numpy.array([1.0, 2.0, 3.0, 1.0]), }
        actual3 = vertex_analysis.reduce_to_all(test3)
        self._test_dict_to_numpy_array_equal(actual3, expected3)

    def test_reduce_to_in_out(self):
        test1 = [('ULSA', 1.0), ('UENG', 2.0), ('UKIN', 3.0), ('ULSA', 1.0)]
        expected1 = { 
                'ULSA': numpy.array([1.0, 1.0]), 
                'not ULSA': numpy.array([2.0, 3.0]),
        }
        actual1 = vertex_analysis.reduce_to_in_out(test1, 'ULSA')
        self._test_dict_to_numpy_array_equal(actual1, expected1)

        test2 = []
        expected2 = {
                'ULSA': numpy.array([]), 
                'not ULSA': numpy.array([]),
        }

        actual2 = vertex_analysis.reduce_to_in_out(test2, 'ULSA')
        self._test_dict_to_numpy_array_equal(actual2, expected2)

        test3 = [('ULSA', 1.0), ('ULSA', 2.0), ('ULSA', 3.0), ('ULSA', 1.0)]
        expected3 = { 
                'ULSA': numpy.array([1.0, 2.0, 3.0, 1.0]), 
                'not ULSA': numpy.array([]),
        }
        actual3 = vertex_analysis.reduce_to_in_out(test3, 'ULSA')
        self._test_dict_to_numpy_array_equal(actual3, expected3)


if __name__ == '__main__':
    unittest.main()
