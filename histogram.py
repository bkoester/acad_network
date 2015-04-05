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
    orig_handles, orig_labels = subfig.get_legend_handles_labels()
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
    histtype = 'step' if len(segments.keys()) > 1 else 'bar'
    cm = matplotlib.pyplot.get_cmap('gist_rainbow')
    sorted_segments = sorted(segments.items(), key=lambda x: x[0])
    subfig.hist([data for key, data in sorted_segments],
            args.bins, histtype=histtype,
            color=[cm(1. * i / len(segments.keys())) for i in
            range(len(segments.keys()))], 
            label=[''.join([key, r', $n = ', str(len(data)), r'$'])  
                for key, data in sorted_segments], **kwargs)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Analyze data dealing with'
        'networks.')
    parser.add_argument('weightedness', 
        help='Either "weighted" or "unweighted"')
    parser.add_argument('-b', '--bins', type=int, default=500,
        help='Number of bins to use with histogram.')
    parser.add_argument('--no-segment', dest='segment', action='store_false', 
            help='Turn off data segmenting the data or not.')
    parser.add_argument('--no-unnormalized', dest='unnormalized', 
            action='store_false', help='Don\'t show the unnormalized histogram.')
    parser.add_argument('--threshold', type=int, default=500,
            help='Filter segments with fewer than this many data points.')
    parser.set_defaults(segment=True)

    args = parser.parse_args()

    if args.weightedness not in ['weighted', 'unweighted']:
        raise ValueError('Weightedness "%s" not known!' % args.weightedness)

    # segment the data filter out small segments
    segments = data_processing.segment_data(sys.stdin, args.segment)
    filtered_segments = {segment: values for segment, values in segments.items()
            if values.size >= args.threshold}
    
    # set up the histogram for various colors
    size = (12, 8) if args.segment and args.unnormalized else (12, 4)
    fig = matplotlib.pyplot.figure(figsize=size)
    #fig.subplots_adjust(hspace=0.4)

    # Create the non-normalized histogram.
    if args.unnormalized:
        if args.segment:
            ax1 = fig.add_subplot(2, 1, 1)
        else:
            ax1 = fig.add_subplot(1, 1, 1)
        create_histogram(ax1, filtered_segments)
        ax1.set_xlabel(' '.join([args.weightedness.title(), 'degree']))
        ax1.set_ylabel('Frequency')
        ax1.set_title(
                ' '.join([args.weightedness.title(), 'degree distribution']))
        handles, labels = sort_legend(ax1)

    # create the normalized histogram if there is segmentation
    if args.segment:
        if args.unnormalized:
            ax2 = fig.add_subplot(2, 1, 2)
        else:
            ax2 = fig.add_subplot(1, 1, 1)
        create_histogram(ax2, filtered_segments, normed=True)
        ax2.set_xlabel(' '.join([args.weightedness.title(), 'degree']))
        ax2.set_ylabel('Normalized frequency')
        ax2.set_title(' '.join(['Normalized', args.weightedness.lower(), 
                                'degree distribution']))

        handles, labels = sort_legend(ax2)

    # create the legend
    matplotlib.pyplot.figlegend(handles, labels, loc='right')

    matplotlib.pyplot.tight_layout()
    matplotlib.pyplot.show()
