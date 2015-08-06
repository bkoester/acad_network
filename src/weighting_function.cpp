#include "weighting_function.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

#include "course.hpp"
#include "student.hpp"


using std::accumulate;
using std::back_inserter; using std::begin; using std::end;
using std::set_intersection;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;

using boost::make_optional; using boost::optional;

using weighting_func_ptr = optional<double>(*)(const Student&, const Student&);


const unordered_map<string, weighting_func_ptr> descriptor_to_weighting_func{
	{"CreditHoursOverEnrollment", CreditHoursOverEnrollment},
	{"InverseEnrollment", InverseEnrollment},
};


static vector<const Course*> GetCoursesInCommon(const Student& student1,
												const Student& student2);


weighting_func_ptr WeightingFuncFactory(string descriptor) {
	return descriptor_to_weighting_func.at(descriptor);
}


optional<double> CreditHoursOverEnrollment(const Student& student1,
										   const Student& student2) {
	auto courses_in_common = GetCoursesInCommon(student1, student2);

	// If there are no courses in common, there's no edge.
	if (courses_in_common.empty()) { return boost::none; }

	// Calculate the connection between the two students.
	double connection{accumulate(begin(courses_in_common),
			end(courses_in_common), 0.,
			[](double connection, const Course* course) {
				return connection + (course->num_credits() /
					static_cast<double>(course->GetNumStudentsEnrolled()));
			})};

	return boost::make_optional<double>(connection);
}


optional<double> InverseEnrollment(const Student& student1,
										   const Student& student2) {
	auto courses_in_common = GetCoursesInCommon(student1, student2);

	// If there are no courses in common, there's no edge.
	if (courses_in_common.empty()) { return boost::none; }

	// Calculate the connection between the two students.
	double connection{accumulate(begin(courses_in_common),
			end(courses_in_common), 0.,
			[](double connection, const Course* course) {
				return connection + (1.0 / course->GetNumStudentsEnrolled());
			})};

	return boost::make_optional<double>(connection);
}


// Returns a list of the courses two students have in common.
vector<const Course*> GetCoursesInCommon(const Student& student1,
										 const Student& student2) {
	const set<const Course*>& student1_courses(student1.courses_taken());
	const set<const Course*>& student2_courses(student2.courses_taken());

	// Create a list of the courses the students have in common.
	vector<const Course*> courses_in_common;
	set_intersection(begin(student1_courses), end(student1_courses),
					 begin(student2_courses), end(student2_courses),
					 back_inserter(courses_in_common));

	return courses_in_common;
}
