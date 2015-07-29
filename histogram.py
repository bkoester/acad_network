"""Simply puts given numbers into a graph."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import argparse
import sys

import matplotlib.pyplot


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Put data into histogram with basic configuration.')
    parser.add_argument(
            'file', nargs='?', type=argparse.FileType('r'),
            default=sys.stdin, help='File to segment (default standard input).')
    parser.add_argument(
            '-b', '--bins', type=int, default=500,
            help='Number of bins to use with histogram.')
    parser.add_argument(
            '--threshold', type=int, default=0,
            help='Filter segments with fewer than this many data points.')
    parser.add_argument(
            '-x', '--x-axis', dest='x_axis', help='Label of the x-axis.')
    parser.add_argument(
            '-y', '--y-axis', dest='y_axis', help='Label of the y-axis.')
    parser.add_argument(
            '-t', '--title', dest='title', help='Title of the graph.')

    args = parser.parse_args()

    data = []
    for line in args.file:
        data.append(float(line))

    fig = matplotlib.pyplot.figure(figsize=(12, 4))
    subfig = fig.add_subplot(1, 1, 1)
    subfig.hist(
            data, args.bins, histtype='bar', color='red',
            label=['$n = {}$'.format(len(data))])

    if args.x_axis:
        subfig.set_xlabel(args.x_axis)
    if args.y_axis:
        subfig.set_ylabel(args.y_axis)
    if args.title:
        subfig.set_title(args.title)

    handles, labels = subfig.get_legend_handles_labels()
    matplotlib.pyplot.figlegend(handles, labels, loc='lower left')

    matplotlib.pyplot.tight_layout()
    matplotlib.pyplot.show()
