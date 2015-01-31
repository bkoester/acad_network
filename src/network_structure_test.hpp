#ifndef STUDENT_NETWORK_TEST_H
#define STUDENT_NETWORK_TEST_H

class CourseNetwork;
class StudentNetwork;

// Declare these functions here so it can also be used to test the structure of
// the network built in graph_builder_test
void TestCourseNetworkStructure(const CourseNetwork& network);
void TestStudentNetworkStructure(const StudentNetwork& network);

#endif  // STUDENT_NETWORK_TEST_H
