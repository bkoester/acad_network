#include "utility.hpp"

#include <iostream>


using std::istream;


void SkipLine(istream& input) { while (input.get() != '\n'); }
