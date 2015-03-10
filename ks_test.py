"""Performs a 2 sample K-S test on pairs of segments."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import itertools
import scipy.stats
import sys

import data_processing


if __name__ == '__main__':
    # put all the points from the distributions into a numpy array
    segments = data_processing.segment_data(sys.stdin, True)

    for segment1, segment2 in itertools.combinations(segments, 2):
        ks_stat, p_val = scipy.stats.ks_2samp(
                segments[segment1], segments[segment2])
        print('%s vs. %s: KS statistic = %1.10d, p-value = %1.10d' % (
            segment1, segment2, ks_stat, p_val))

