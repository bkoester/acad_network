#include "graph_builder.hpp"

#include <sstream>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"
#include "student_container_mock.hpp"
#include "student_network.hpp"
#include "test_data_streams.hpp"


using std::stringstream;

using ::testing::AtLeast;
using ::testing::Const;
using ::testing::Return;
using ::testing::ReturnRef;

using boost::optional;
using boost::make_optional;


static optional<double> TestWeightingFunc(const Student&, const Student&);


TEST(GraphBuilderTest, BuildStudentNetworkFromStudents) {
	// make the students
	auto course1 = Course{"ENGLISH", 125, 0, 4};
	auto course2 = Course{"AAPTIS", 277, 0, 4};
	auto course3 = Course{"CHEM", 210, 0, 4};
	auto course4 = Course{"CHEM", 211, 0, 1};
	auto course5 = Course{"ENVIRON", 311, 0, 4};
	auto course6 = Course{"MATH", 425, 0, 3};

	Student student1{147195};
	student1.AddCoursesTaken({&course5, &course2, &course1});
	Student student2{312995};
	student2.AddCoursesTaken({&course1, &course3, &course4, &course2});
	Student student3{352468};
	student3.AddCoursesTaken({&course1, &course5, &course6});
	Student student4{500928};
	student4.AddCoursesTaken({&course1, &course3});
	Student student5{567890};
	student5.AddCourseTaken(&course6);
	// put them in a container
	MockStudentContainer students;
	EXPECT_CALL(Const(students), Find(147195)).Times(AtLeast(1)).WillRepeatedly(
			ReturnRef(student1));
	EXPECT_CALL(Const(students), Find(312995)).Times(AtLeast(1)).WillRepeatedly(
			ReturnRef(student2));
	EXPECT_CALL(Const(students), Find(352468)).Times(AtLeast(1)).WillRepeatedly(
			ReturnRef(student3));
	EXPECT_CALL(Const(students), Find(500928)).Times(AtLeast(1)).WillRepeatedly(
			ReturnRef(student4));
	EXPECT_CALL(Const(students), Find(567890)).Times(AtLeast(1)).WillRepeatedly(
			ReturnRef(student5));
	ON_CALL(students, size()).WillByDefault(Return(5));

	students.Insert({student1, student2, student3, student4, student5});

	course1.AddStudentEnrolled(student1.id());
	course1.AddStudentEnrolled(student2.id());
	course1.AddStudentEnrolled(student3.id());
	course1.AddStudentEnrolled(student4.id());
	course2.AddStudentEnrolled(student1.id());
	course2.AddStudentEnrolled(student2.id());
	course3.AddStudentEnrolled(student2.id());
	course3.AddStudentEnrolled(student4.id());
	course4.AddStudentEnrolled(student2.id());
	course5.AddStudentEnrolled(student1.id());
	course5.AddStudentEnrolled(student3.id());
	course6.AddStudentEnrolled(student3.id());
	course6.AddStudentEnrolled(student5.id());

	StudentNetwork network{
		BuildStudentNetworkFromStudents(students, TestWeightingFunc)};

	StudentNetwork::vertex_t student1_d{
		network.GetVertexDescriptor(Student::Id{147195})};
	StudentNetwork::vertex_t student2_d{
		network.GetVertexDescriptor(Student::Id{312995})};
	StudentNetwork::vertex_t student3_d{
		network.GetVertexDescriptor(Student::Id{352468})};
	StudentNetwork::vertex_t student4_d{
		network.GetVertexDescriptor(Student::Id{500928})};
	StudentNetwork::vertex_t student5_d{
		network.GetVertexDescriptor(Student::Id{567890})};

	EXPECT_THROW(network.Get(student1_d, student1_d), NoEdgeException);
	EXPECT_DOUBLE_EQ(3.0, network.Get(student1_d, student2_d));
	EXPECT_DOUBLE_EQ(2.0, network.Get(student1_d, student3_d));
	EXPECT_DOUBLE_EQ(1.0, network.Get(student1_d, student4_d));
	EXPECT_THROW(network.Get(student1_d, student5_d), NoEdgeException);

	EXPECT_THROW(network.Get(student2_d, student2_d), NoEdgeException);
	EXPECT_DOUBLE_EQ(3.0, network.Get(student2_d, student3_d));
	EXPECT_DOUBLE_EQ(2.0, network.Get(student2_d, student4_d));
	EXPECT_THROW(network.Get(student2_d, student5_d), NoEdgeException);

	EXPECT_THROW(network.Get(student3_d, student3_d), NoEdgeException);
	EXPECT_DOUBLE_EQ(3.0, network.Get(student3_d, student4_d));
	EXPECT_THROW(network.Get(student3_d, student5_d), NoEdgeException);

	EXPECT_THROW(network.Get(student4_d, student4_d), NoEdgeException);
	EXPECT_THROW(network.Get(student4_d, student5_d), NoEdgeException);

	EXPECT_THROW(network.Get(student5_d, student5_d), NoEdgeException);
}


optional<double> TestWeightingFunc(const Student& student1,
								   const Student& student2) {
	if ((student1 == Student{147195} && student2 == Student{147195}) ||
			(student2 == Student{147195} && student1 == Student{147195})) {
		return boost::none;
	} else if ((student1 == Student{147195} && student2 == Student{312995}) ||
			(student2 == Student{147195} && student1 == Student{312995})) {
		return make_optional(3.);
	} else if ((student1 == Student{147195} && student2 == Student{352468}) ||
			(student2 == Student{147195} && student1 == Student{352468})) {
		return make_optional(2.);
	} else if ((student1 == Student{147195} && student2 == Student{500928}) ||
			(student2 == Student{147195} && student1 == Student{500928})) {
		return make_optional(1.);
	} else if ((student1 == Student{147195} && student2 == Student{567890}) ||
			(student2 == Student{147195} && student1 == Student{567890})) {
		return boost::none;
	} else if ((student1 == Student{312995} && student2 == Student{312995}) ||
			(student2 == Student{312995} && student1 == Student{312995})) {
		return boost::none;
	} else if ((student1 == Student{312995} && student2 == Student{352468}) ||
			(student2 == Student{312995} && student1 == Student{352468})) {
		return make_optional(3.);
	} else if ((student1 == Student{312995} && student2 == Student{500928}) ||
			(student2 == Student{312995} && student1 == Student{500928})) {
		return make_optional(2.);
	} else if ((student1 == Student{312995} && student2 == Student{567890}) ||
			(student2 == Student{312995} && student1 == Student{567890})) {
		return boost::none;
	} else if ((student1 == Student{352468} && student2 == Student{352468}) ||
			(student2 == Student{352468} && student1 == Student{352468})) {
		return boost::none;
	} else if ((student1 == Student{352468} && student2 == Student{500928}) ||
			(student2 == Student{352468} && student1 == Student{500928})) {
		return make_optional(3.);
	} else if ((student1 == Student{352468} && student2 == Student{567890}) ||
			(student2 == Student{352468} && student1 == Student{567890})) {
		return boost::none;
	} else if ((student1 == Student{500928} && student2 == Student{500928}) ||
			(student2 == Student{500928} && student1 == Student{500928})) {
		return boost::none;
	} else if ((student1 == Student{500928} && student2 == Student{567890}) ||
			(student2 == Student{500928} && student1 == Student{567890})) {
		return boost::none;
	} else if ((student1 == Student{567890} && student2 == Student{567890}) ||
			(student2 == Student{567890} && student1 == Student{567890})) {
		return boost::none;
	}
	assert(false);
}
