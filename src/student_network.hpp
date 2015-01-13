#ifndef STUDENT_NETWORK_H
#define STUDENT_NETWORK_H

#include "network.hpp"
#include "student.hpp"

class StudentNetwork : public Network<Student, int> {
 public:
	StudentNetwork(const graph_t& graph) : Network{graph} {}
};


#endif  // STUDENT_NETWORK_H
