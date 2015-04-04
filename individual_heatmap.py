"""Performs a basic reading of data from a tsv."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import collections
import numpy
import matplotlib.pyplot
import sys

import data_processing
import student_analysis


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Analyze interactions of an'
            'individual student')
    parser.add_argument('-f', '--student-file', dest='student_path', 
            default='../data/student.tab',
            help='The file containing edge interactions.')
    parser.add_argument('field', 
        help='The field for which to analyze interactions.')
    parser.add_argument('weightedness', 
        help='Either "weighted" or "unweighted"')

    args = parser.parse_args()

    # get the weights of individual segments
    reduced_segments = data_processing.segment_data(sys.stdin)
    segment_weights = {key: sum(values) for (
        key, values in reduced_segments.items())}
    
    # get expected weights for segments
    with open(args.student_path, 'r') as student_file:
        segment_counts = student_analysis.get_segment_counts(
                student_file, args.field)

    # put into a numpy matrix
    population = sum(segment_counts.values())
    total_weight = sum(segment_weights.values())
    segments = sorted(segment_counts.keys())

    heatarray = numpy.zeros((1, len(segments)))
    for segment, actual_weight in segment_weights.items():
        expected_weight = (segment_counts[segment] / population) * total_weight
        heatarray[0, segments.index(segment)] = actual_weight - expected_weight


    print(heatarray)
    # make the heat array
    fig, ax = matplotlib.pyplot.subplots()
    heatmap = matplotlib.pyplot.pcolor(
            heatarray, cmap=matplotlib.pyplot.cm.RdBu)
    heatmap.set_clim(vmin=-1.0, vmax=1.0)

    # where the ticks are located
    ax.invert_yaxis()
    ax.xaxis.tick_top()

    # tick locations relative to the cell
    ax.set_xticks(numpy.arange(heatarray.shape[1]) + 0.5, minor=False)
    ax.set_yticks(numpy.arange(1) + 0.5, minor=False)
    # stop a white bar from appearing due to rounding
    ax.set_xlim(right=len(segments))
    ax.set_ylim(bottom=1)

    # tick labels
    ax.set_xticklabels(segments, minor=False)
    ax.set_yticklabels(["Student"], minor=False)

    # rotation of x-axis
    matplotlib.pyplot.xticks(rotation=90)

    # labels
    matplotlib.pyplot.xlabel(
        'An individual student\'s %s interactions from ...' % 
        (args.weightedness.title()))
    ax.xaxis.set_label_position('top')
    matplotlib.pyplot.ylabel('To ...')

    matplotlib.pyplot.colorbar(heatmap)

    # put the numbers in the boxes
    for y in range(heatarray.shape[0]):
        for x in range(heatarray.shape[1]):
            color = 'black' if abs(heatarray[y, x]) < 1 else 'white'
            matplotlib.pyplot.text(
                    x + 0.5, y + 0.5, '%.2f' % heatarray[y, x],
                    horizontalalignment='center',
                    verticalalignment='center',
                    color=color)

    fig.tight_layout()
    matplotlib.pyplot.show()
    
