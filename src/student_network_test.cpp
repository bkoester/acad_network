#include "student_network.hpp"

#include <algorithm>
#include <sstream>

#include <boost/graph/adjacency_list.hpp>
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
using boost::add_vertex;


class StudentNetworkTest : public ::testing::Test {
 public:
	 void SetUp() override {
		 // add vertices
		 auto student1 = add_vertex(StudentId{312995}, graph_);
		 auto student2 = add_vertex(StudentId{500928}, graph_);
		 auto student3 = add_vertex(StudentId{147195}, graph_);
		 auto student4 = add_vertex(StudentId{352468}, graph_);
		 auto student5 = add_vertex(StudentId{567890}, graph_);

		 // add edges
		 add_edge(student1, student2, 1, graph_);
		 add_edge(student1, student3, 1, graph_);
		 add_edge(student1, student4, 1, graph_);

		 add_edge(student2, student3, 1, graph_);
		 add_edge(student2, student4, 1, graph_);

		 add_edge(student3, student4, 1, graph_);
		 add_edge(student4, student5, 1, graph_);

		 network = StudentNetwork{graph_};
	 }

 private:
	StudentNetwork::graph_t graph_;

   // must be defined after the private member for initialization reasons
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
