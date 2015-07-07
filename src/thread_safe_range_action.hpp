#ifndef THREAD_SAFE_RANGE_ACTION_H
#define THREAD_SAFE_RANGE_ACTION_H

#include <algorithm>
#include <iterator>
#include <thread>
#include <vector>

#include "utility.hpp"


const int minimum_range_size{2};


template <typename Function, typename It, typename... Args>
void PerformFunctionOnRange(Function function, It begin_it, It end_it, 
		Args... args) {
	// split the range into number of threads ranges
	auto total_size = std::distance(begin, end);
	auto range_size = total_size / num_threads;
	auto range_extra = total_size % num_threads;

	// Create the first range, place the items resulting from the modulus of
	// total_size and num_threads on this range.
	It range_begin_it{begin_it};
	It range_end_it{begin_it}; 
	std::advance(range_end_it, range_size + range_extra);

	std::vector<std::thread> threads;
	// Constructing the loop like this removes the overhead of constructing
	// threads on ranges with 0 elements.
	while (range_begin_it != end_it) {
		// Create the thread.
		threads.emplace_back(
				function, current_begin_it, current_end_it, args...);
		// Advance the iterators. Only advance the end iterator if it doesn't go
		// past the end.
		range_begin_it = range_end_it;
		if (range_end_it != end_it) { std::advance(end_it, range_size); }
	}

	// Wait for all the threads to complete.
	std::for_each(std::begin(threads), std::end(threads), 
			std::bind(&thread::join, std::placeholders::_1));
};


#endif  // THREAD_SAFE_RANGE_ACTION_H
