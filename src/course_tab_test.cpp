#include "course_tab.hpp"

#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "course.hpp"
#include "student.hpp"

using std::string;
using std::stringstream;
using std::vector;

const string sample_tab{
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


class CourseTabTest : public ::testing::Test {
 public:
	 CourseTabTest() : 
		 course_tab_stream_{sample_tab}, course_tab{course_tab_stream_} {}
	
 private:
	stringstream course_tab_stream_;

 protected:
	CourseTab course_tab;
};

TEST_F(CourseTabTest, Iteration) {
	auto course_tab_it = course_tab.begin();
	
	// test classes are the same
	CourseTab::Line line1{Course{"ENGLISH", 125}, StudentId{312995}};
	CourseTab::Line result{*course_tab_it++};
	EXPECT_EQ(line1, result);
	CourseTab::Line line2{Course{"CHEM", 210}, StudentId{312995}};
	EXPECT_EQ(line2, *course_tab_it++);
	CourseTab::Line line3{Course{"CHEM", 211}, StudentId{312995}};
	EXPECT_EQ(line3, *course_tab_it++);
	CourseTab::Line line4{Course{"AAPTIS", 277}, StudentId{312995}};
	EXPECT_EQ(line4, *course_tab_it++);
	CourseTab::Line line5{Course{"ENGLISH", 125}, StudentId{500928}};
	EXPECT_EQ(line5, *course_tab_it++);
	CourseTab::Line line6{Course{"CHEM", 210}, StudentId{500928}};
	EXPECT_EQ(line6, *course_tab_it++);
	CourseTab::Line line7{Course{"ENGLISH", 125}, StudentId{147195}};
	EXPECT_EQ(line7, *course_tab_it++);
	CourseTab::Line line8{Course{"ENVIRON", 311}, StudentId{147195}};
	EXPECT_EQ(line8, *course_tab_it++);
	CourseTab::Line line9{Course{"AAPTIS", 277}, StudentId{147195}};
	EXPECT_EQ(line9, *course_tab_it++);
	CourseTab::Line line10{Course{"ENGLISH", 125}, StudentId{352468}};
	EXPECT_EQ(line10, *course_tab_it++);
	CourseTab::Line line11{Course{"ENVIRON", 311}, StudentId{352468}};
	EXPECT_EQ(line11, *course_tab_it++);
	CourseTab::Line line12{Course{"MATH", 425}, StudentId{352468}};
	EXPECT_EQ(line12, *course_tab_it++);

	// make sure the iterator has hit the end of the input
	EXPECT_EQ(course_tab.end(), course_tab_it);
}
