#ifndef UTILITY_H
#define UTILITY_H

#include <iosfwd>
#include <memory>
#include <string>
#include <utility>


enum class NetworkType_e { Course, Student };


// The number of threads to help build the network. Defined as extern to allow
// change by command line options.
extern int num_threads;


std::ostream& operator<<(
		std::ostream& output, const NetworkType_e& network_type);

std::istream& operator>>(std::istream& input, NetworkType_e& network_type);


template <typename Enum>
constexpr auto ToIntegralType(Enum e)
	-> typename std::underlying_type<Enum>::type {
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}


// Ignore any line in the given istream
void SkipLine(std::istream& input);

// Skip a field delimited by a tab.
void SkipTabField(std::istream& input);


template <typename InputIt>
bool HasIntersection(
		InputIt first1, InputIt last1, InputIt first2, InputIt last2) {
	using namespace std::rel_ops;
	// go through the first container
	while (first1 != last1) {
		// go through the second container until the elements are no longer
		// smaller than the current element in the first container and the
		// container hasn't ended
		while (first2 != last2 && *first2 < *first1) { ++first2; }
		if (first2 == last2) { return false; }
		// if the elements are equal, we're good
		if (*first1 == *first2) { return true; }
		// otherwise, keep going in the first container
		++first1;
	}
	return false;
}


bool icompare(const std::string& first, const std::string& second);


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>{new T{std::forward<Args>(args)...}};
}


#endif  // UTILITY_H
