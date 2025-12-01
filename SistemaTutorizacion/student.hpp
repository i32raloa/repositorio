#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>
#include <vector>

class Student {
    private:
        std::string id, nombre, apellido, carrera, facultad;
        std::vector<std::string> idiomas;
        std::string idtutor="0";
    public:
        Student() = default;
        Student(std::string id, std::string nombre, std::string apellido, std::string carrera, std::string facultad, std::vector<std::string> idiomas, std::string idtutor = "0")
            : id(id), nombre(nombre), apellido(apellido), carrera(carrera), facultad(facultad), idiomas(idiomas) , idtutor(idtutor) {}
        
        Student(const std::vector<std::string>& row);

        std::string getId() const { return id; }
        std::string getNombre() const { return nombre; }
        std::string getApellido() const { return apellido; }
        std::string getCarrera() const { return carrera; }
        std::string getFacultad() const { return facultad; }
        std::vector<std::string> getIdiomas() const { return idiomas; }
        std::string getIdTutor() const { return idtutor; }

        void setIdTutor(const std::string& newIdTutor) { idtutor = newIdTutor; }

        std::vector<std::string> toCSV() const;
};

#endif