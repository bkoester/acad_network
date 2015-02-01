#include "utility.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>


using std::equal;
using std::istream;
using std::string;
using std::tolower;


void SkipLine(istream& input) { while (input.get() != '\n'); }

bool icompare(const std::string& first, const std::string& second) {
    return first.size() == second.size() &&
		equal(first.begin(), first.end(), second.begin(), 
			[](char c1, char c2) { return tolower(c1) == tolower(c2); });
}
