#ifndef WEIGHTING_FUNCTION_H
#define WEIGHTING_FUNCTION_H


#include <string>

#include <boost/optional.hpp>


class Student;


// Defines weighting functions for graph connections.
// Every function returns an optional double. If the valeu is not specified,
// then the edge is empty.

// Returns a pointer to a weighting function. Unfortunately, function pointers
// must be used to allow weighting functions to be chosen at runtime.
boost::optional<double>(
		*WeightingFuncFactory(std::string descriptor))
(const Student&, const Student&);


// Calculates the weight between students as the summation of credits / size of
// class for every class in which they are coenrolled.
boost::optional<double> CreditHoursOverEnrollment(const Student& student1,
												  const Student& student2);


// Just use the inverse of enrollment, analogous to what Mark Newman describes
// in his paper examining the scientific coauthorship:
// http://www-personal.umich.edu/~mejn/papers/016132.pdf
boost::optional<double> InverseEnrollment(const Student& student1,
										  const Student& student2);

#endif  // WEIGHTING_FUNCTION_H
