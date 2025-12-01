#include "tutor.hpp"
#include "student.hpp"
#include "AutoAssigner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <vector>
#include <limits>


std::vector<std::vector<std::string>> readCSV(const std::string& filename){
    std::vector<std::vector<std::string>> data;

    std::ifstream file(filename);
    if(!file.is_open()){
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while(std::getline(file, line)){
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> row;
        while(std::getline(ss, token, ',')){
            row.push_back(token);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

std::map<std::string, std::vector<std::string>> getAreaMap(const std::string& filename) {
    std::map<std::string, std::vector<std::string>> areaMap;
    auto rows = readCSV(filename);
    for(const auto& row : rows) {
        if(row.size() < 2) continue;
        std::vector<std::string> carreras;
        std::stringstream ss(row[1]);
        std::string carrera;
        while(std::getline(ss, carrera, ';')) {
            carreras.push_back(carrera);
        }
        areaMap[row[0]] = carreras;
    }
    return areaMap;
}

void writeStudentsCSV(const std::string& filename, const std::vector<Student>& students) {
    std::ofstream file(filename);
    if(!file.is_open()){
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    for(const auto& student : students){
        auto row = student.toCSV();
        for(size_t i = 0; i < row.size(); ++i){
            file << row[i];
            if(i < row.size() - 1){
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}

void mostrarAsignaciones(const std::vector<Student>& students, const std::vector<Tutor>& tutors) {
    std::cout << "\n===== Asignaciones de Tutores =====\n";
    for(const auto& student : students){
        std::string tutorName = "Sin asignar";
        for(const auto& tutor : tutors){
            if(tutor.getId() == student.getIdTutor()){
                tutorName = tutor.getNombre() + " " + tutor.getApellido();
                break;
            }
        }
        std::cout << "Estudiante: " << student.getNombre() << " " << student.getApellido()
        << " -> Tutor: " << tutorName << "\n";
    }
    std::cout << "===================================\n";
}

void mostrarNuevosAsignados(const std::vector<Student>& pendientes, const std::vector<Student>& todos, const std::vector<Tutor>& tutors) {
    if (pendientes.empty()) {
        std::cout << "\nNo hay ningún estudiante pendiente de asignar tutor.\n";
        return;
    }

    std::cout << "\n===== Estudiantes asignados automáticamente =====\n";
    for (const Student& student : todos) {
        std::string tutorName;
        for (const Tutor& tutor : tutors) {
            if (tutor.getId() == student.getIdTutor()) {
                tutorName = tutor.getNombre() + " " + tutor.getApellido();
                break;
            }
        }
        std::cout << "Estudiante: " << student.getNombre() << " " << student.getApellido()
                  << " -> Tutor: " << tutorName << "\n";
    }
    std::cout << "==============================================\n";
}


int main() {
    std::srand(std::time(nullptr));

    std::vector<Student> students;
    std::vector<Tutor> tutors;
    std::map<std::string, std::vector<std::string>> areaMap;

    try {
        auto studentRows = readCSV("students.csv");
        for(const auto& row : studentRows) students.push_back(Student(row));

        auto tutorRows = readCSV("tutors.csv");
        for(const auto& row : tutorRows) tutors.push_back(Tutor(row));

        areaMap = getAreaMap("area.csv");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    int opcion = 0;
    AutoAssigner assigner;

    do {
        std::cout << "\n\n\n";
        std::cout << "\n================================ MENÚ PRINCIPAL ================================\n\n\n\n";
        std::cout << "\t\t         1. Mostrar estudiantes y tutores\n\n";
        std::cout << "\t\t        2. Asignar tutores automáticamente\n\n";
        std::cout << "\t\t\t      3. Salir del programa\n\n\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;




        std::vector<Student> pendientes;

        switch(opcion) {
            case 1:
                mostrarAsignaciones(students, tutors);
                
                break;
            case 2:
                for (const Student& s : students) {
                    if (s.getIdTutor()=="0") pendientes.push_back(s);
                }

                assigner.assignTutors(students, tutors, areaMap);
                writeStudentsCSV("students.csv", students);

                mostrarNuevosAsignados(pendientes, students, tutors);
                break;
            case 3:
                std::cout << "Saliendo del programa...\n";
                break;
            default:
                std::cout << "Opción no válida, intente nuevamente.\n";
        }
    } while(opcion != 3);

    return 0;
}