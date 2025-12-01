#ifndef TUTOR_HPP
#define TUTOR_HPP

#include <string>
#include <vector>

class Tutor {
    private:
        std::string id, nombre, apellido, area, facultad;
        std::vector<std::string> idiomas;
    
    public:
        Tutor() = default;

        Tutor(std::string id, std::string nombre, std::string apellido, std::string area, std::string facultad, std::vector<std::string> idiomas)
            : id(id), nombre(nombre), apellido(apellido), area(area), facultad(facultad), idiomas(idiomas) {}

        Tutor(const std::vector<std::string>& row);

        std::string getId() const { return id; }
        std::string getNombre() const { return nombre; }
        std::string getApellido() const { return apellido; }
        std::string getArea() const { return area; }
        std::string getFacultad() const { return facultad; }
        std::vector<std::string> getIdiomas() const { return idiomas; }

        std::vector<std::string> toCSV() const;
};

#endif