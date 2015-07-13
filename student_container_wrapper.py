"""Wraps student_container swig module."""


__author__ = "karepker@umich.edu Kar Epker"
__copyright__ = "Kar Epker, 2015"


import sys

from cpp_iterator_wrapper import CppIteratorWrapper


class StudentContainerWrapper:
    """Wraps the StudentContainer swig class.
    
    Consumers of this class should use self.students.
    """

    SEGMENTERS = {
        'gender': lambda s: s.GetGenderDescription(),
        'ethnicity': lambda s: s.GetEthnicityDescription(),
        'major1': lambda s: s.GetMajor1Description(),
        'school': lambda s: s.school(),
        'transfer': lambda s: 'Transfer' if s.transfer() else 'Non-transfer',
        'first_year': lambda s: s.first_term // 100,
        }

    def __init__(self, swig_module_path, student_archive_path):
        """Imports the swig module and performs initial calls.

        Args:
            swig_module_path (string): Path to the swig modules.
            student_archive_path (string): Path to the student boost archive.
        """

        sys.path.append(swig_module_path)
        import student_container
        self.students = student_container.StudentContainer_LoadFromArchive(
            student_archive_path)


    def __iter__(self):
        return CppIteratorWrapper(self.students.begin(), self.students.end())

    def load_courses(self, courses):
        """Loads the courses the students took from course container.
        
        Args:
            courses (swig CourseContainer): A swig CourseContainer object
            holding the courses
        """

        self.students.UpdateCourses(courses)


    def segment_student(self, student_id, segmenter):
        """Segments the student with the given segmenter.

        Args:
            student_id (int): The id of the student to segment.
            segmenter (callable): A function-esque object that takes a student.
        """

        student = self.students.Find(student_id)
        return segmenter(student)
