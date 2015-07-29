"""Plots the degree distribution of students given a file."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import operator
import sys

import matplotlib.pyplot

from readable_directory import ReadableDirectory
from student_container_wrapper import StudentContainerWrapper
import vertex_analysis


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


def create_histogram(subfig, bins, segments, **kwargs):
    """Creates a histogram with the correct format for the given subfigure.

    Args:
        subfig (subfigure object): The subfigure object for which to create a
            histogram.
        bins (int): The number of bins with which to create this histogram.
        segments (dict(string => numpy.arrray(float))): The segments to plot on
            the histogram.
        kwargs (dict): Other arguments to give to the histogram creation.

    Returns:
        A sorted list of handles and labels.
    """
    num_segments = len(segments.keys())
    histtype = 'step' if num_segments > 1 else 'bar'
    cm = matplotlib.pyplot.get_cmap('gist_rainbow')
    sorted_segments = sorted(segments.items(), key=lambda x: x[0])
    subfig.hist(
            [data for key, data in sorted_segments], bins, histtype=histtype,
            color=[cm(1. * i / num_segments) for i in range(num_segments)],
            label=['{}, $n = {}$'.format(key, len(data))
                   for key, data in sorted_segments],
            **kwargs)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Analyze data dealing with networks.')
    parser.add_argument(
            'file', nargs='?', type=argparse.FileType('r'),
            default=sys.stdin, help='File to segment containing data to put in '
            'histogram (default standard input).')
    parser.add_argument(
            '-s', '--segmenter', default='all',
            choices=StudentContainerWrapper.SEGMENTERS.keys(),
            help='Segmenting function to use.')
    parser.add_argument(
            '-b', '--bins', type=int, default=500,
            help='Number of bins to use with histogram.')
    parser.add_argument(
            '-d', '--distributions', nargs='*',
            choices=['frequency', 'normalized'], default=['frequency'],
            help='Determines which distributions to show: The default '
            'frequency distribution or the normalized distribution (or both) '
            '(default: only frequency distribution).')
    parser.add_argument(
            '--threshold', type=int, default=0,
            help='Filter segments with fewer than this many data points.')
    parser.add_argument(
            '-x', '--x-axis', dest='x_axis', help='Label of the x-axis.')
    parser.add_argument(
            '-y', '--y-axis', dest='y_axis', help='Label of the y-axis.')
    parser.add_argument(
            '-t', '--title', dest='title', help='Title of the graph.')
    parser.add_argument(
            '--student-archive-path', dest='student_archive_path',
            help='Path of the student archive file.', required=True)
    parser.add_argument(
            '--swig-module-path', dest='swig_module_path',
            help='Directory containing the swig modules.',
            action=ReadableDirectory, required=True)

    parser.set_defaults()

    args = parser.parse_args()

    show_unnormalized = 'frequency' in args.distributions
    show_normalized = 'normalized' in args.distributions

    # segment the data
    vertex_lines = vertex_analysis.get_id_values(args.file)
    students_wrapper = StudentContainerWrapper(
            args.swig_module_path, args.student_archive_path)
    segmenter = StudentContainerWrapper.SEGMENTERS[args.segmenter]
    mapped_lines = vertex_analysis.map_to_segments(
            vertex_lines, segmenter, students_wrapper)
    segments = vertex_analysis.reduce_to_segments(mapped_lines)

    # filter out small segments
    filtered_segments = {segment: values for segment, values in segments.items()
                         if values.size >= args.threshold}

    # set up the histogram for various colors
    size = ((12, 8) if show_unnormalized and show_normalized
            else (12, 4))
    fig = matplotlib.pyplot.figure(figsize=size)
    #fig.subplots_adjust(hspace=0.4)

    # Create the non-normalized histogram.
    if show_unnormalized:
        if show_normalized:
            ax1 = fig.add_subplot(2, 1, 1)
        else:
            ax1 = fig.add_subplot(1, 1, 1)
        create_histogram(ax1, args.bins, filtered_segments)
        if args.x_axis:
            ax1.set_xlabel(args.x_axis)
        if args.y_axis:
            ax1.set_ylabel(args.y_axis)
        if args.title:
            ax1.set_title(args.title)
        handles, labels = sort_legend(ax1)

    # create the normalized histogram if there is segmentation
    if show_normalized:
        if show_unnormalized:
            ax2 = fig.add_subplot(2, 1, 2)
        else:
            ax2 = fig.add_subplot(1, 1, 1)
        create_histogram(ax2, args.bins, filtered_segments, normed=True)
        if args.x_axis:
            xlabel = args.x_axis
            ax2.set_xlabel(args.x_axis)
        if args.y_axis:
            ax2.set_ylabel(args.y_axis)
        if args.title:
            ax2.set_title(args.title)
        handles, labels = sort_legend(ax2)

    # create the legend
    matplotlib.pyplot.figlegend(handles, labels, loc='best')

    matplotlib.pyplot.tight_layout()
    matplotlib.pyplot.show()
