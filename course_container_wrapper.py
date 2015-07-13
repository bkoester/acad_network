"""Wraps course_container swig module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import sys

from cpp_iterator_wrapper import CppIteratorWrapper

class CourseContainerWrapper:
    """Wraps the CourseContainer swig class
    

    Consumers of this class should use self.courses.
    """

    def __init__(self, swig_module_path, course_archive_path):
        """Imports the swig module and performs initial calls.

        Args:
            swig_module_path (string): Path to the swig modules.
            course_archive_path (string): Path to the course boost archive.
        """
        sys.path.append(swig_module_path)
        import course_container
        self.courses = course_container.CourseContainer_LoadFromArchive(
            course_archive_path)

    def __iter__(self):
        return CppIteratorWrapper(self.courses.begin(), self.courses.end())

