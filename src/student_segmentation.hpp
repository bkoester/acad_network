#ifndef STUDENT_SEGMENTATION_H
#define STUDENT_SEGMENTATION_H

#include <functional>
#include <iostream>
#include <string>

#include "student.hpp"
#include "student_network.hpp"
#include "tab_reader.hpp"


class StudentSegmenter {
 public:

	class Segment;
	class VertexData;

	void AddSegment(
			std::ostream& save_stream, 
			std::function<std::string(const Student&)> segment_func, 
			std::function<std::string(const VertexData& data)> datum_func);
	void AddSegment(Segment&& segment);

	void RunSegmentation(const StudentNetwork& network,
						 const student_container_t& students);

	struct VertexData {
		VertexData(StudentNetwork::vertex_t vertex_d, 
				   const StudentNetwork& network,
				   const student_container_t& students);

		Student student;
		double weighted_degree;
		double weighted_degree_norm;
		int unweighted_degree;
	};
		
	class Segment{
	 public:
		Segment(std::ostream& save_stream, 
				std::function<std::string(const Student& student)> segment_func,
				std::function<std::string(const VertexData& data)> datum_func);
		// functions made virtual for mocking
		std::string GetSegment(const Student& student) const
		{ return segment_function_(student); }
		std::string GetDatum(const VertexData& data) const
		{ return datum_function_(data); }

		std::ostream& save_stream() { return save_stream_; }

	 private:
		friend class StudentSegmentationTest;
		std::function<std::string(const Student& student)> segment_function_;
		std::function<std::string(const VertexData& data)> datum_function_;
		std::ostream& save_stream_;
	};

 private:
	friend class StudentSegmentationTest;
	std::vector<Segment> segments_;
};


#endif  // STUDENT_SEGMENTATION_H
