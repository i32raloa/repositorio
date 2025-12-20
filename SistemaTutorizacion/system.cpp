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

void mostrarAsignaciones(const std::vector<Student>& students,
                         const std::vector<Tutor>& tutors) {

    std::cout << "\n==================== ASIGNACIONES DE TUTORES ====================\n";

    for (const auto& student : students) {

        const Tutor* tutorAsignado = nullptr;

        for (const auto& tutor : tutors) {
            if (tutor.getId() == student.getIdTutor()) {
                tutorAsignado = &tutor;
                break;
            }
        }

        std::cout << "\nEstudiante:\n";
        std::cout << "  ID      : " << student.getId() << "\n";
        std::cout << "  Nombre  : " << student.getNombre()
                  << " " << student.getApellido() << "\n";
        std::cout << "  Carrera : " << student.getCarrera() << "\n\n";

        std::cout << "Tutor:\n";

        if (tutorAsignado) {
            std::cout << "  ID      : " << tutorAsignado->getId() << "\n";
            std::cout << "  Nombre  : " << tutorAsignado->getNombre()
                      << " " << tutorAsignado->getApellido() << "\n";
            std::cout << "  Área    : " << tutorAsignado->getArea() << "\n";
        } else {
            std::cout << "  Estado  : Sin asignar\n";
        }

        std::cout << "---------------------------------------------------------------\n";
    }

    std::cout << "=================================================================\n";
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

    std::cout << "\nSeleccione el número del estudiante al que desea asignar un tutor\n\n";
    size_t studentIndex;
    std::cin >> studentIndex;
    if(studentIndex < 1 || studentIndex > students.size()){
        std::cout << "\nÍndice de estudiante inválido.\n";
        return;
    }

    std::cout << "\n\nLista de tutores:\n";
    for(size_t j = 0; j < tutors.size(); j++){
        std::cout << j + 1 << ". " << tutors[j].getNombre() << " " << tutors[j].getApellido() << "\n";
    }
    std::cout << "\nSeleccione el número del tutor que desea asignar:\n\n";
    size_t tutorIndex;
    std::cin >> tutorIndex;
    if(tutorIndex < 1 || tutorIndex > tutors.size()){
        std::cout << "\nÍndice de tutor inválido.\n";
        return;
    }

    students[studentIndex - 1].setIdTutor(tutors[tutorIndex - 1].getId());
    std::cout << "\n\nTutor asignado exitosamente.\n";

}

void iniciarChat(const std::string& miId,const std::vector<Student>& students, const std::vector<Tutor>& tutors) {
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

    if(!mostrarChatsDisponibles(miId, students, tutors)) {
        std::cout << "\nNo hay chats disponibles para iniciar.\n";
        close(sock);
        return;
    }

    int numeroDest;
    std::cout << "\nSeleccione el número del tutor/estudiante con el que quieres chatear: ";
    std::cin >> numeroDest;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string idDest = formatearIdDestino(numeroDest, students, tutors, miId);
    if(idDest.empty()) {
        std::cout << "Número inválido, saliendo del chat.\n";
        close(sock);
        return;
    }


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

    std::cout << "\nPulse intro para conversar y escriba SALIR para salir del chat.\n> ";

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


std::string seleccionarTutor(const std::vector<Tutor>& tutors) {
    std::cout << "\n===== TUTORES =====\n";
    for (size_t i = 0; i < tutors.size(); ++i) {
        std::cout << i + 1 << ". "
                  << tutors[i].getNombre() << " "
                  << tutors[i].getApellido()
                  << " (ID " << tutors[i].getId() << ")\n";
    }

    int opcion;
    std::cout << "Seleccione su tutor: ";
    std::cin >> opcion;

    if (opcion < 1 || opcion > (int)tutors.size()) {
        throw std::runtime_error("Selección inválida");
    }

    return tutors[opcion - 1].getId();
}



std::string seleccionarEstudiante(const std::vector<Student>& students) {
    std::cout << "\n===== ESTUDIANTES =====\n";
    for (size_t i = 0; i < students.size(); ++i) {
        std::cout << i + 1 << ". "
                  << students[i].getNombre() << " "
                  << students[i].getApellido()
                  << " (ID " << students[i].getId() << ")\n";
    }

    int opcion;
    std::cout << "Seleccione su estudiante: ";
    std::cin >> opcion;

    if (opcion < 1 || opcion > (int)students.size()) {
        throw std::runtime_error("Selección inválida");
    }

    return students[opcion - 1].getId();
}



std::string obtenerTutorDelEstudiante(const std::string& idEstudiante, const std::vector<Student>& students) {

    for (const auto& s : students) {
        if (s.getId() == idEstudiante) {
            return s.getIdTutor(); // puede ser "0"
        }
    }
    return "0";
}


std::vector<std::string> obtenerEstudiantesDelTutor(const std::string& idTutor, const std::vector<Student>& students) {

    std::vector<std::string> resultado;

    for (const auto& s : students) {
        if (s.getIdTutor() == idTutor) {
            resultado.push_back(s.getId());
        }
    }
    return resultado;
}


bool mostrarChatsDisponibles(const std::string& miId, const std::vector<Student>& students, const std::vector<Tutor>& tutors) {

    bool esTutor = false;

    for (const auto& t : tutors) {
        if (t.getId() == miId) {
            esTutor = true;
            break;
        }
    }

    std::cout << "\n================= CHATS DISPONIBLES =================\n";

    if (esTutor) {
        std::cout << "Rol: Tutor\n";
        std::cout << "Estudiantes tutorizados:\n";

        auto lista = obtenerEstudiantesDelTutor(miId, students);

        if (lista.empty()) {
            std::cout << "  (No tienes estudiantes asignados)\n";
            return false;
        } else {
            for (const auto& id : lista) {
                std::cout << "  - Estudiante ID: " << id << "\n";
            }
            return true;
        }
    } else {
        std::cout << "Rol: Estudiante\n";
        std::string idTutor = obtenerTutorDelEstudiante(miId, students);

        if (idTutor == "0") {
            std::cout << "  (No tienes tutor asignado)\n";
            return false;
        } else {
            std::cout << "  Tutor asignado: ID " << idTutor << "\n";
        }
        return true;
    }

    std::cout << "====================================================\n";
}


std::string formatearIdDestino(int numero, const std::vector<Student>& students, const std::vector<Tutor>& tutors, const std::string& miId) {

    if(miId[0] == 'S') {
        if(numero >= 1 && numero <= (int)tutors.size()) {
            return tutors[numero-1].getId(); 
        }
    }

    else if(miId[0] == 'T') {

        std::vector<Student> tutorizados;
        for(const auto& s : students) {
            if(s.getIdTutor() == miId) tutorizados.push_back(s);
        }
        if(numero >= 1 && numero <= (int)tutorizados.size()) {
            return tutorizados[numero-1].getId();
        }
    }
    return "";
}
