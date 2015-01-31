#include "graph_builder.hpp"

#include <sstream>

#include "gtest/gtest.h"

#include "course.hpp"
#include "network_structure_test.hpp"
#include "student.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"


using std::stringstream;


class GraphBuilderTest : public ::testing::Test {
 public:
	GraphBuilderTest() : enrollment_stream{enrollment_tab} {}

 protected:
	stringstream enrollment_stream;
};


TEST_F(GraphBuilderTest, BuildStudentNetworkFromEnrollment) { 
	StudentNetwork network{BuildStudentNetworkFromEnrollment(enrollment_stream)};
	TestStudentNetworkStructure(network);
}


TEST_F(GraphBuilderTest, BuildStudentNetworkFromStudents) { 
	// make the students
	Student student1{147195};
	student1.AddCoursesTaken({Course{"ENVIRON", 311}, Course{"AAPTIS", 277}, 
			Course{"ENGLISH", 125}});
	Student student2{312995};
	student2.AddCoursesTaken({Course{"ENGLISH", 125}, Course{"CHEM", 210}, 
			Course{"CHEM", 211}, Course{"AAPTIS", 277}});
	Student student3{352468};
	student3.AddCoursesTaken({Course{"ENGLISH", 125}, Course{"ENVIRON", 311},
			Course{"MATH", 425}});
	Student student4{500928};
	student4.AddCoursesTaken({Course{"ENGLISH", 125}, Course{"CHEM", 210}});
	Student student5{567890};
	student5.AddCourseTaken(Course{"MATH", 425});

	// put them in a container
	student_container_t students{
		student1, student2, student3, student4, student5};

	StudentNetwork network{BuildStudentNetworkFromStudents(students)};
	TestStudentNetworkStructure(network);
}
