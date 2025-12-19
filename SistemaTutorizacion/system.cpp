#include "system.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>


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

    std::cout << "\n===== ESTUDIANTES ASIGNADOS AUTOMÁTICAMENTE =====\n";
    for(const Student& pendiente: pendientes) {
        for(const Student& actual: todos) {
            if(pendiente.getId() == actual.getId()) {

                std::string tutorName = "Sin asignar";
                for(const auto& tutor : tutors){
                    if(tutor.getId() == actual.getIdTutor()){
                        tutorName = tutor.getNombre() + " " + tutor.getApellido();
                        break;
                    }
                }
                std::cout << "Estudiante: " << actual.getNombre() << " " << actual.getApellido() << "->tutor: " << tutorName << "\n";
            }
        }
    }
    std::cout << "==============================================\n";
}

void AsignacionManual(std::vector<Student>& students, const std::vector<Tutor> tutors){
    std::cout << "\n===== ASIGNACIÓN MANUAL DE TUTORES =====\n\n";

    std::cout << "Lista de estudiantes:\n";
    for(size_t i = 0; i < students.size(); i++){
        std::cout << i + 1 << ". " << students[i].getNombre() << " " << students[i].getApellido();
        if(students[i].getIdTutor() == "0"){
            std::cout << "   (Pendiente de asignar tutor)\n";
        }
        else{
            std::cout << "   (Tutor actual: " << students[i].getIdTutor() << ")\n";
        }
    }

    std::cout << "Seleccione el número del estudiante al que desea asignar un tutor\n";
    size_t studentIndex;
    std::cin >> studentIndex;
    if(studentIndex < 1 || studentIndex > students.size()){
        std::cout << "Índice de estudiante inválido.\n";
        return;
    }

    std::cout << "Lista de tutores:\n";
    for(size_t j = 0; j < tutors.size(); j++){
        std::cout << j + 1 << ". " << tutors[j].getNombre() << " " << tutors[j].getApellido() << "\n";
    }
    std::cout << "Seleccione el número del tutor que desea asignar:\n";
    size_t tutorIndex;
    std::cin >> tutorIndex;
    if(tutorIndex < 1 || tutorIndex > tutors.size()){
        std::cout << "Índice de tutor inválido.\n";
        return;
    }

    students[studentIndex - 1].setIdTutor(tutors[tutorIndex - 1].getId());
    std::cout << "Tutor asignado exitosamente.\n";

}

void iniciarChat(const std::string& miId) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creando socket");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error conectando al servidor de chat");
        close(sock);
        return;
    }

    send(sock, miId.c_str(), miId.size(), 0);

    std::string idDest;
    std::cout << "\nID del tutor/estudiante con el que quieres chatear: ";
    std::cin >> idDest;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::thread receptor([&]() {
        char buffer[1024];
        while (true) {
            int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) break;
            buffer[bytes] = '\0';
            std::cout << "\n" << buffer << "\n> ";
            std::cout.flush();
        }
    });

    std::cout << "\nPulse intro para conversar y control+c para salir del chat.\n> ";

    while (true) {
        std::string mensaje;
        std::getline(std::cin, mensaje);

        if (mensaje == "SALIR") break;

        std::string data = idDest + "|" + mensaje;
        send(sock, data.c_str(), data.size(), 0);
        std::cout << "> ";
    }

    close(sock);
    receptor.detach();
}
