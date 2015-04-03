#include "course_container.hpp"

#include "course.hpp"

#include "gmock/gmock.h"


class MockCourseContainer : public CourseContainer {
 public:
	MOCK_METHOD0(begin, container_t::iterator());
	MOCK_CONST_METHOD0(begin, container_t::const_iterator());
	MOCK_CONST_METHOD0(cbegin, container_t::const_iterator());
	MOCK_METHOD0(end, container_t::iterator());
	MOCK_CONST_METHOD0(end, container_t::const_iterator());
	MOCK_CONST_METHOD0(cend, container_t::const_iterator());
};
