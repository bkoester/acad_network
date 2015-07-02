%module student
%{
#include "student.hpp"
%}

class Student {
 public:
    typedef int Id;

    Student();
    Student(Id);

    int id();
};
