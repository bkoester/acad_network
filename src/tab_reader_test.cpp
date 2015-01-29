#include "tab_reader.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"


using std::binary_search;
using std::string;
using std::stringstream;


static stringstream enrollment_stream{
"ID\tSUBJECT\tCATALOGNBR\tCOURSE_CODE\tGRADE\tGPAO\tCUM_GPA\tTOTALCREDITS\t"
"TOTALGRADEPTS\tCOURSECREDIT\tTERM\n"
"312995\tENGLISH\t125\tNA\t3.7\t3.65909090909091\t3.66538461538462\t26\t95.3\t"
"4\t201403\n"
"312995\tCHEM\t210\tCHEM_III\t3.3\t3.73181818181818\t3.66538461538462\t26\t95.3"
"\t4\t201403\n"
"312995\tCHEM\t211\tCHEM_III_LAB\t3\t3.692\t3.66538461538462\t26\t95.3\t1"
"\t201407\n"
"312995\tAAPTIS\t277\tNA\t4\t3.60454545454545\t3.66538461538462\t26\t95.3\t4\t"
"201403\n"
"500928\tENGLISH\t125\tNA\t4\t3.57538461538462\t3.58181818181818\t66\t236.4\t1"
"\t201407\n"
"500928\tCHEM\t210\tCHEM_III\t3.3\t3.73181818181818\t3.66538461538462\t26\t95.3"
"\t4\t201403\n"
"147195\tENGLISH\t125\tNA\t4\t3.57538461538462\t3.58181818181818\t66\t236.4\t1"
"\t201403\n"
"147195\tENVIRON\t311\tNA\t4\t3.55483870967742\t3.58181818181818\t66\t236.4\t4"
"\t201405\n"
"147195\tAAPTIS\t277\tNA\t4\t3.60454545454545\t3.66538461538462\t26\t95.3\t4\t"
"201403\n"
"352468\tENGLISH\t125\tNA\t4\t3.57538461538462\t3.58181818181818\t66\t236.4\t1"
"\t201405\n"
"352468\tENVIRON\t311\tNA\t4\t3.55483870967742\t3.58181818181818\t66\t236.4\t4"
"\t201403\n"
"352468\tMATH\t425\tNA\t4\t4\t4\t27\t108\t3\t201407\n"
};

static stringstream students_stream{
"ID\tGENDER\tETHNICITY\tFIRST_TERM\tDEGREE_TERM\tTRANSFER\tMAJOR1\tMAJOR2\t"
"MAJOR1_LONG\tMAJOR2_LONG\tFIRST_DECLARE\tFIRST_DECLARE_LONG\t"
"FIRST_DECLARE_TERM\tPELL_STATUS\tACT_ENGL\tACT_MATH\tACT_COMP\tCOA\n"
"147195\tF\t2\t200607\t201003\tN\t9.0101\t42.2704\t"
"Communication Studies/Speech C\tExperimental Psychology\tNA\tNA\tNA\tN\tNA\tNA"
"\tNA\tULSA\n"
"312995\tF\t3\t200807\t201205\tN\t42.2706\tNA\tPhysiological Psychology/Psych\tNA\t"
"42.2706\tPhysiological Psychology/Psych\t201103\tN\t24\t25\t24\tULSA\n"
"352468\tF\t1\t201207\t201303\tN\tNA\tNA\tNA\tNA\tNA\tNA\tNA\tN\t36\t36\t35\t"
"NA\n"
"500928\tF\t6\t200707\t201103\tN\t16.0905\t45.0201\t"
"Spanish Language and Literatur\tAnthropology\t16.0905\tSpanish Language and "
"Literatur\t200807\tN\tNA\tNA\tNA\tULSA\n"
};

TEST(TabRecordTest, ReadStudents) {
	auto students = ReadStudents(students_stream);

	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{312995}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{500928}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{147195}));
	EXPECT_TRUE(binary_search(
				students.begin(), students.end(), Student{352468}));

	EXPECT_EQ(4u, students.size());
}

TEST(TabRecordTest, ReadEnrollment) {
	student_container_t students{Student{147195}, Student{312995}, 
		Student{352468}, Student{500928}};

	auto courses = ReadEnrollment(enrollment_stream, students);

	Course english125{*courses.find(Course{"ENGLISH", 125})};
	Course chem210{*courses.find(Course{"CHEM", 210})};
	Course chem211{*courses.find(Course{"CHEM", 211})};
	Course aaptis277{*courses.find(Course{"AAPTIS", 277})};
	Course environ311{*courses.find(Course{"ENVIRON", 311})};
	Course math425{*courses.find(Course{"MATH", 425})};

	EXPECT_EQ(1u, courses.count(english125));
	EXPECT_EQ(1u, courses.count(chem210));
	EXPECT_EQ(1u, courses.count(chem211));
	EXPECT_EQ(1u, courses.count(aaptis277));
	EXPECT_EQ(1u, courses.count(environ311));
	EXPECT_EQ(1u, courses.count(math425));

	Student student1{*lower_bound(
			students.begin(), students.end(), Student{312995})};
	Student student2{*lower_bound(
			students.begin(), students.end(), Student{500928})};
	Student student3{*lower_bound(
			students.begin(), students.end(), Student{147195})};
	Student student4{*lower_bound(
			students.begin(), students.end(), Student{352468})};

	EXPECT_TRUE(student1.HasTakenCourse(english125));
	EXPECT_TRUE(student1.HasTakenCourse(chem210));
	EXPECT_TRUE(student1.HasTakenCourse(chem211));
	EXPECT_TRUE(student1.HasTakenCourse(aaptis277));
	EXPECT_FALSE(student1.HasTakenCourse(environ311));
	EXPECT_FALSE(student1.HasTakenCourse(math425));

	EXPECT_TRUE(student2.HasTakenCourse(english125));
	EXPECT_TRUE(student2.HasTakenCourse(chem210));
	EXPECT_FALSE(student2.HasTakenCourse(chem211));
	EXPECT_FALSE(student2.HasTakenCourse(aaptis277));
	EXPECT_FALSE(student2.HasTakenCourse(environ311));
	EXPECT_FALSE(student2.HasTakenCourse(math425));

	EXPECT_TRUE(student3.HasTakenCourse(english125));
	EXPECT_FALSE(student3.HasTakenCourse(chem210));
	EXPECT_FALSE(student3.HasTakenCourse(chem211));
	EXPECT_TRUE(student3.HasTakenCourse(aaptis277));
	EXPECT_TRUE(student3.HasTakenCourse(environ311));
	EXPECT_FALSE(student3.HasTakenCourse(math425));

	EXPECT_TRUE(student4.HasTakenCourse(english125));
	EXPECT_FALSE(student4.HasTakenCourse(chem210));
	EXPECT_FALSE(student4.HasTakenCourse(chem211));
	EXPECT_FALSE(student4.HasTakenCourse(aaptis277));
	EXPECT_TRUE(student4.HasTakenCourse(environ311));
	EXPECT_TRUE(student4.HasTakenCourse(math425));
}
