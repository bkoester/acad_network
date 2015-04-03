#include "student_container.hpp"

#include <vector>

#include "student.hpp"

#include "gmock/gmock.h"


class MockStudentContainer : public StudentContainer {
 public:
	MOCK_CONST_METHOD0(size, std::vector<Student>::size_type());
	MOCK_METHOD1(Find, Student&(Student::Id));
	MOCK_CONST_METHOD1(Find, const Student&(Student::Id));
};
