"""Tests for segment module."""

__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import unittest
import unittest.mock

import segment


class SegmentTest(unittest.TestCase):
    """Tests segment module."""

    def test_get_segment_counts(self):
        """Tests get_segment_counts."""

        segmenter_name = (
                'student_container_wrapper.StudentContainerWrapper.SEGMENTERS')

        # test 1 (normal test case)
        with unittest.mock.patch(segmenter_name) as segmenter_mock1:
            segmenter_mock1.__getitem__.return_value = unittest.mock.MagicMock(
                    side_effect=['UENG', 'UENG', 'ULSA'])

            student_container_mock1 = unittest.mock.MagicMock()
            student_container_mock1.__iter__.return_value = [1, 2, 3]

            expected1 = {
                    'UENG': 2,
                    'ULSA': 1
                    }
            actual1 = segment.get_segment_counts(
                    student_container_mock1, 'school')
            self.assertEqual(actual1, expected1)

        # test 2 (empty container)
        with unittest.mock.patch(segmenter_name) as segmenter_mock2:
            segmenter_mock2.__getitem__.return_value = unittest.mock.MagicMock(
                    side_effect=[])

            student_container_mock2 = unittest.mock.MagicMock()
            student_container_mock2.__iter__.return_value = []

            expected2 = {}
            actual2 = segment.get_segment_counts(
                    student_container_mock2, 'school')
            self.assertEqual(actual2, expected2)



if __name__ == '__main__':
    unittest.main()
