#ifndef COURSE_NETWORK_H
#define COURSE_NETWORK_H

#include <iosfwd>
#include <unordered_map>

#include "network.hpp"
#include "course.hpp"

class CourseNetwork : public Network<Course, int> {
 public:
	CourseNetwork() {}
	// input must be a boost graph archive
	CourseNetwork(std::istream& input);
	CourseNetwork(const graph_t& graph);

	// gets the vertex type in the graph for a specific course
	// will throw out_of_range exception for the vertex
	vertex_t GetVertex(const Course& course) const;

	void Load(std::istream& input_graph_archive);

	int operator()(const vertex_t& source, 
				   const vertex_t& target) const override;

 private:
	// Fill out the hash table of courses => vertices.
	void BuildCourseToVertexMap();

	std::unordered_map<Course, vertex_t, CourseHasher> course_to_vertex_;
};

std::ostream& operator<<(
		std::ostream& output, const CourseNetwork& course_network);

#endif  // COURSE_NETWORK_H
