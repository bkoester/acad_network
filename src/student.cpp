#include "student.hpp"

#include <iostream>


using std::ostream;


ostream& operator<<(ostream& os, const Student& student) {
	if (student.id()) { os << "ID: Undefined"; }
	else { os << "ID: " << student.id(); }
	return os;
}
