#include "course_network.hpp"

#include <iostream>

#include <boost/graph/adjacency_list.hpp>

#include "course.hpp"


using std::istream; using std::ostream;


const int default_edge_value{0};


CourseNetwork::CourseNetwork(istream& input) : Network{input}
{ BuildCourseToVertexMap(); }


CourseNetwork::CourseNetwork(const graph_t& graph) : Network{graph}
{ BuildCourseToVertexMap(); }


CourseNetwork::vertex_t CourseNetwork::GetVertex(const Course& course) const
{ return course_to_vertex_.at(course); }


const Course& CourseNetwork::operator[](const Course& course) const
{ return Network<Course, int>::operator[](GetVertex(course)); }


Course& CourseNetwork::operator[](const Course& course)
{ return Network<Course, int>::operator[](GetVertex(course)); }


int CourseNetwork::CalculateValue(
		const vertex_t& source, const vertex_t& target) const {
	auto edge_option = GetEdgeDescriptor(source, target);
	if (!edge_option) { return default_edge_value; }
	return Network<Course, int>::operator[](edge_option.get());
}


void CourseNetwork::Load(std::istream& input_graph_archive) {
	// load the graph itself using the base function
	Network<Course, int>::Load(input_graph_archive);
	BuildCourseToVertexMap();
}


void CourseNetwork::BuildCourseToVertexMap() {
	for (const auto& vertex : GetVertexDescriptors())
	{ course_to_vertex_[operator[](vertex)] = vertex; }
}


ostream& operator<<(ostream& output, const CourseNetwork& course_network) {
	// serialize graph to output
	course_network.Save(output);
	return output;
}
