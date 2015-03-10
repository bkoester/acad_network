"""Plots the degree distribution of students given a file."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import csv
import matplotlib.pyplot
import numpy
import operator
import sys

import data_processing


def sort_legend(subfig):
    """Sorts the labels in the legend.

    Args:
        subfig (subfigure object): The subfigure object whose axis to sort.

    Returns:
        A sorted list of handles and labels.
    """
    orig_handles, orig_labels = ax1.get_legend_handles_labels()
    return zip(*sorted(zip(orig_handles, orig_labels), 
                       key=operator.itemgetter(1)))


def create_histogram(subfig, segments, **kwargs):
    """Creates a histogram with the correct format for the given subfigure.

    Args:
        subfig (subfigure object): The subfigure object for which to create a
            histogram.
        segments (dict(string => numpy.arrray(float))): The segments to plot on 
            the histogram.
        kwargs (dict): Other arguments to give to the histogram creation.

    Returns:
        A sorted list of handles and labels.
    """

    subfig.hist(list(segments.values()),
            args.bins, histtype='step',
            color=[cm(1. * i / len(segments)) for i in range(len(segments))], 
            label=[''.join([key, r', $n = ', str(len(data)), r'$'])  
                for key, data in segments.items()], **kwargs)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Analyze data dealing with'
        'networks.')
    parser.add_argument('-b', '--bins', type=int, default=50,
        help='Number of bins to use with histogram.')
    parser.add_argument('-x', '--x-axis', dest='x_axis', default='X axis', 
            help='Assign the name of the x axis.')
    parser.add_argument('-y', '--y-axis', dest='y_axis', default='y axis', 
            help='Assign the name of the y axis.')
    parser.add_argument('-t', '--title', default='Title',
        help='Assign the title.')
    parser.add_argument('--no-segment', dest='segment', help='Turn off data '
        'segmenting the data or not.')
    parser.add_argument('--threshold', type=int, default=500,
        help='Filter plots with fewer than this many data points.')
    parser.set_defaults(segment=True)

    args = parser.parse_args()

    # segment the data filter out small segments
    segments = data_processing.segment_data(sys.stdin, args.segment)
    filtered_segments = {segment: values for segment, values in segments.items()
            if values.size >= args.threshold}
    
    # set up the histogram for various colors
    cm = matplotlib.pyplot.get_cmap('gist_rainbow')
    fig = matplotlib.pyplot.figure(figsize=(12, 8))
    fig.subplots_adjust(hspace=0.4)

    # Create the non-normalized histogram.
    ax1 = fig.add_subplot(2, 1, 1)
    create_histogram(ax1, filtered_segments)
    ax1.set_xlabel(args.x_axis)
    ax1.set_ylabel(args.y_axis)
    ax1.set_title(args.title)

    # Create the normalized histogram.
    ax2 = fig.add_subplot(2, 1, 2)
    create_histogram(ax2, filtered_segments, normed=True)
    ax2.set_xlabel(args.x_axis)
    ax2.set_ylabel(' '.join(['Normalized', args.y_axis.lower()]))
    ax2.set_title(' '.join(['Normalized', args.title.lower()]))

    # create the legend
    handles, labels = sort_legend(ax1)
    matplotlib.pyplot.figlegend(handles, labels, loc='right')
    matplotlib.pyplot.show()
