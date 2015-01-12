#include "course_network.hpp"

#include <string>
#include <sstream>
#include <utility>

#include <boost/graph/adjacency_list.hpp>

#include "gtest/gtest.h"

#include "graph_builder.hpp"

using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::stringstream;

using boost::edge;

void TestGraphStructure(const CourseNetwork& network);

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

/* In an easier to read format:
ID	SUBJECT	CATALOGNBR	COURSE_CODE	GRADE	GPAO	CUM_GPA	TOTALCREDITS	TOTALGRADEPTS	COURSECREDIT	TERM
312995	ENGLISH	125	NA	3.7	3.65909090909091	3.66538461538462	26	95.3	4	201403
312995	CHEM	210	CHEM_III	3.3	3.73181818181818	3.66538461538462	26	95.3	4	201403
312995	CHEM	211	CHEM_III_LAB	3	3.692	3.66538461538462	26	95.3	1	201407
312995	AAPTIS	277	NA	4	3.60454545454545	3.66538461538462	26	95.3	4	201403
500928	ENGLISH	125	NA	4	3.57538461538462	3.58181818181818	66	236.4	1	201407
500928	CHEM	210	CHEM_III	3.3	3.73181818181818	3.66538461538462	26	95.3	4	201403
147195	ENGLISH	125	NA	4	3.57538461538462	3.58181818181818	66	236.4	1	201403
147195	ENVIRON	311	NA	4	3.55483870967742	3.58181818181818	66	236.4	4	201405
147195	AAPTIS	277	NA	4	3.60454545454545	3.66538461538462	26	95.3	4	201403
352468	ENGLISH	125	NA	4	3.57538461538462	3.58181818181818	66	236.4	1	201405
352468	ENVIRON	311	NA	4	3.55483870967742	3.58181818181818	66	236.4	4	201403
352468	MATH	425	NA	4	4	4	27	108	3	201407
*/

class CourseNetworkTest : public ::testing::Test {
 public:
	CourseNetworkTest() : course_stream_{sample_tab},
						  network{BuildGraphFromCourseTab(course_stream_)} {}

 private:
	stringstream course_stream_;

	// must be defined after the private member for initialization reasons
 protected:
	CourseNetwork network;
};

TEST_F(CourseNetworkTest, Construction) { TestGraphStructure(network); }

TEST_F(CourseNetworkTest, Serialization) {
	// save the network
	stringstream archive;
	network.Save(archive);
	TestGraphStructure(network);

	// load the network
	CourseNetwork loaded_network;
	loaded_network.Load(archive);
	TestGraphStructure(loaded_network);
}

void TestGraphStructure(const CourseNetwork& network) {
	CourseNetwork::vertex_t english125_vertex{network.GetVertex(
			Course{"ENGLISH", 125})};
	CourseNetwork::vertex_t chem210_vertex{network.GetVertex(
			Course{"CHEM", 210})};
	CourseNetwork::vertex_t chem211_vertex{network.GetVertex(
			Course{"CHEM", 211})};
	CourseNetwork::vertex_t aaptis277_vertex{network.GetVertex(
			Course{"AAPTIS", 277})};
	CourseNetwork::vertex_t environ311_vertex{network.GetVertex(
			Course{"ENVIRON", 311})};
	CourseNetwork::vertex_t math425_vertex{network.GetVertex(
			Course{"MATH", 425})};

	// test edges that exist
	EXPECT_EQ(2, network(english125_vertex, chem210_vertex));

	// test both ways for this one
	EXPECT_EQ(2, network(chem210_vertex, english125_vertex));

	EXPECT_EQ(1, network(english125_vertex, chem211_vertex));
	EXPECT_EQ(2, network(english125_vertex, aaptis277_vertex));
	EXPECT_EQ(2, network(english125_vertex, environ311_vertex));

	EXPECT_EQ(1, network(chem210_vertex, chem211_vertex));
	EXPECT_EQ(1, network(chem210_vertex, aaptis277_vertex));
	EXPECT_EQ(0, network(chem210_vertex, environ311_vertex));
	// test non-existent edge both ways
	EXPECT_EQ(0, network(environ311_vertex, chem210_vertex));

	EXPECT_EQ(1, network(chem211_vertex, aaptis277_vertex));
	EXPECT_EQ(0, network(chem211_vertex, environ311_vertex));
	EXPECT_EQ(0, network(chem211_vertex, math425_vertex));
	
	EXPECT_EQ(1, network(aaptis277_vertex, environ311_vertex));
	EXPECT_EQ(0, network(aaptis277_vertex, math425_vertex));

	EXPECT_EQ(1, network(environ311_vertex, math425_vertex));

}
