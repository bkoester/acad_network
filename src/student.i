%module student

%include "std_string.i"

%{
#include "student.hpp"
%}

class Student {
 public:
    typedef int Id;

	enum class Gender { Male, Female, Unspecified };
	enum class Ethnicity { Hispanic_only, American_Indian, Asian, Black, 
		Pacific_Islander, White, Multiracial, Unknown, Undocumented };

    Student();
    Student(Id);

    int id() const;
	Gender gender() const;
	Ethnicity ethnicity() const;
	int first_term() const;
	int degree_term() const;
	bool transfer() const;
	std::string school() const;

	std::string GetMajor1Description() const;
	std::string GetMajor2Description() const;
    std::string GetGenderDescription() const;
    std::string GetEthnicityDescription() const;
    std::string GetDescription() const;

};
