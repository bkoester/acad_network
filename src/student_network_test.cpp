#include "student_network.hpp"

#include <algorithm>
#include <sstream>

#include <boost/graph/adjacency_matrix.hpp>
#include "gtest/gtest.h"

#include "course.hpp"
#include "graph_builder.hpp"
#include "network.hpp"
#include "network_structure_test.hpp"
#include "student.hpp"
#include "test_data_streams.hpp"


using std::find_if;
using std::stringstream;

using boost::add_edge;
using boost::vertex;


class StudentNetworkTest : public ::testing::Test {
 public:
	 void SetUp() override {
		 StudentNetwork::graph_t graph{5};

		 // add vertices
		 auto student1 = vertex(0, graph);
		 graph[student1] = StudentId{312995};
 		 auto student2 = vertex(1, graph);
		 graph[student2] = StudentId{500928};
 		 auto student3 = vertex(2, graph);
		 graph[student3] = StudentId{147195};
 		 auto student4 = vertex(3, graph);
		 graph[student4] = StudentId{352468};
 		 auto student5 = vertex(4, graph);
		 graph[student5] = StudentId{567890};

		 // add edges
		 add_edge(student1, student2, 1, graph);
		 add_edge(student1, student3, 1, graph);
		 add_edge(student1, student4, 1, graph);

		 add_edge(student2, student3, 1, graph);
		 add_edge(student2, student4, 1, graph);

		 add_edge(student3, student4, 1, graph);
		 add_edge(student4, student5, 1, graph);

		 network = StudentNetwork{graph};
	 }

 protected:
	StudentNetwork network;
};


TEST_F(StudentNetworkTest, Serialization) {
	// save the network
	stringstream archive;
	network.Save(archive);
	TestStudentNetworkStructure(network);

	// load the network
	StudentNetwork loaded_network;
	loaded_network.Load(archive);
	TestStudentNetworkStructure(loaded_network);
}
