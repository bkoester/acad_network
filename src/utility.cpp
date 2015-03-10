#include "utility.hpp"

#include <boost/program_options.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>


using std::equal;
using std::istream; using std::ostream;
using std::string; using std::getline;
using std::tolower;
namespace po = boost::program_options;


// By default, use just 1 thread.
int num_threads{1};


ostream& operator<<(
		ostream& output, const NetworkType_e& network_type) {
	if (network_type == NetworkType_e::Course)
	{ output << "Course"; }
	else if (network_type == NetworkType_e::Student)
	{ output << "Student"; }
	else { assert(false); }

	return output;
}


istream& operator>>(istream& input, NetworkType_e& network_type) {
	// get the string
	string network_input;
	input >> network_input;

	// make sure the network type is valid, throw error if not
	if (icompare(network_input, "course"))
	{ network_type = NetworkType_e::Course; }
	else if (icompare(network_input, "student"))
	{ network_type = NetworkType_e::Student; }
	else { throw po::invalid_option_value{"Invalid network type!"}; }

	return input;
}


void SkipLine(istream& input) { while (input.get() != '\n'); }


void SkipTabField(istream& input) {
	string dummy;
	getline(input, dummy, '\t');
}


bool icompare(const std::string& first, const std::string& second) {
    return first.size() == second.size() &&
		equal(first.begin(), first.end(), second.begin(), 
			[](char c1, char c2) { return tolower(c1) == tolower(c2); });
}
