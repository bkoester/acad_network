"""Provides an iterator implementation wrapping C++ iterators from swig."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


class CppIteratorWrapper:
    """Converts C++ begin() and end() iteration to python style iteration."""

    def __init__(self, begin, end):
        """Constructor for the iterator.
            
        Args:
            begin (PySwigIterator): The begin iterator
            end (PySwigIterator): The end iterator
        """

        self.begin = begin
        self.end = end

    def __iter__(self):
        return self

    def __next__(self):
        if self.begin != self.end:
            value = self.begin.value()
            self.begin.next()
            return value 
        else:
            raise StopIteration()
