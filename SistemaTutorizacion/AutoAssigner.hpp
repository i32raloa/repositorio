#ifndef AUTOASSIGNER_HPP
#define AUTOASSIGNER_HPP

#include "student.hpp"
#include "tutor.hpp"
#include <vector>
#include <string>
#include <map>

class AutoAssigner {
    public:
        AutoAssigner() = default;

        void assignTutors(std::vector<Student>& students, const std::vector<Tutor>& tutors, 
                          const std::map<std::string, std::vector<std::string>>& areaMap);
};

#endif

