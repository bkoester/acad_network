#ifndef STUDENT_H
#define STUDENT_H

#include <iosfwd>

class Student {
 public:
	Student() : id_{-1} {}
	Student(int id) : id_{id} {}

	int id() const { return id_; }

	bool operator==(const Student& other) const { return id() == other.id(); }

 private:
	int id_;
};

std::ostream& operator<<(std::ostream& os, const Student& student);

#endif  // STUDENT_H
