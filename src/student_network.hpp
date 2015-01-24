#ifndef STUDENT_NETWORK_H
#define STUDENT_NETWORK_H

#include "course.hpp"
#include "network.hpp"
#include "student.hpp"


class StudentNetwork : public Network<StudentId, int> {
 public:
	StudentNetwork() {}
	StudentNetwork(const graph_t& graph) : Network{graph} {}
};


#endif  // STUDENT_NETWORK_H
