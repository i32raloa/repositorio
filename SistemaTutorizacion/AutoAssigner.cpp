#include "AutoAssigner.hpp"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if(!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

static int matchScore(const Student& student, const Tutor& tutor, const std::map<std::string, std::vector<std::string>>& areaMap) {
    int score = 0;

    auto it=areaMap.find(tutor.getArea());
    if(it != areaMap.end()){
        const auto& carreras = it->second;
        if(std::find(carreras.begin(), carreras.end(), student.getCarrera()) != carreras.end()) {
            score +=4;
        }
    }


    if(student.getFacultad() == tutor.getFacultad()) score +=3;

    for(const auto& lang : student.getIdiomas()) {
        for(const auto& tlang : tutor.getIdiomas()) {
            if(lang == tlang) {
                score +=2;
            }
        }
    }

    if(student.getApellido()[0] == tutor.getApellido()[0]) score +=1;

    return score;
}

void AutoAssigner::assignTutors(std::vector<Student>& students, const std::vector<Tutor>& tutors,
                               const std::map<std::string, std::vector<std::string>>& areaMap) {
    for(auto& student : students) {
        if(student.getIdTutor() != "0") continue;

        int bestScore = -1;
        std::string bestTutorId = "0";

        for(const auto& tutor : tutors) {
            int score = matchScore(student, tutor, areaMap);
            if(score > bestScore) {
                bestScore = score;
                bestTutorId = tutor.getId();
            }
        }

        if(bestTutorId == "0" && !tutors.empty()) {
            int randomIndex = std::rand() % tutors.size();
            bestTutorId = tutors[randomIndex].getId();
        }
        student.setIdTutor(bestTutorId);
    }
}
