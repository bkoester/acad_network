"""Argparse custom action for accepting a readable directory."""


import argparse
import os


class ReadableDirectory(argparse.Action):
    """Action for acception a readable directory from argparse."""

    def __call__(self, parser, namespace, values, option_string=None):
        if not os.path.isdir(values):
            raise argparse.ArgumentTypeError('%s is not a valid path!' % values)
        if not os.access(values, os.R_OK):
            raise argparse.ArgumentTypeError('%s is not readable!' % values)
        setattr(namespace, self.dest, values)

