#include "student.hpp"
#include <sstream>

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

static std::string join(const std::vector<std::string>& vec, char delimiter) {
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i < vec.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

Student::Student(const std::vector<std::string>& row){
    id = row[0];
    nombre = row[1];
    apellido = row[2];
    carrera = row[3];
    facultad = row[4];
    idiomas = split(row[5], ';');
    idtutor = row[6];
}

std::vector<std::string> Student::toCSV() const{
    return {id, nombre, apellido, carrera, facultad, join(idiomas, ';'), idtutor};
}
