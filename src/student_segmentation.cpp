#include "student_segmentation.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream>
#include <string>

#include "student.hpp"
#include "student_container.hpp"
#include "student_network.hpp"


using std::accumulate;
using std::begin; using std::end;
using std::endl; using std::ostream;
using std::function;
using std::string;


void StudentSegmenter::AddSegment(
		ostream& save_stream, function<string(const Student&)> segment_func, 
		function<string(const VertexData&)> datum_func)
{ segments_.emplace_back(save_stream, segment_func, datum_func); }


void StudentSegmenter::AddSegment(Segment&& segment)
{ segments_.push_back(segment); }


void StudentSegmenter::RunSegmentation(
		const StudentNetwork& network, const StudentContainer& students) {
	for (const auto& vertex_d : network.GetVertexDescriptors()) {
		VertexData data{vertex_d, network, students};
		for (auto& segment : segments_) {
			segment.save_stream() << segment.GetSegment(data.student) << "\t"
								  << segment.GetDatum(data) << endl;
		}
	}
}


StudentSegmenter::VertexData::VertexData(
		StudentNetwork::vertex_t vertex_d, const StudentNetwork& network,
		const StudentContainer& students) :
	student{students.Find(network[vertex_d])} {

	auto out_edges = network.GetOutEdgeValues(vertex_d);
	weighted_degree = accumulate(begin(out_edges), end(out_edges), 0.);
	weighted_degree_norm = weighted_degree / student.GetTotalCreditsTaken();
	unweighted_degree = out_edges.size();
}


StudentSegmenter::Segment::Segment(ostream& save_stream, 
		function<string(const Student& student)> segment_func,
		function<string(const VertexData& data)> datum_func) : 
	segment_function_{segment_func}, datum_function_{datum_func}, 
	save_stream_(save_stream) {}
