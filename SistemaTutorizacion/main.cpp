#include "system.hpp"
#include "AutoAssigner.hpp"
#include <iostream>
#include <ctime>

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
    int rol=0;
    AutoAssigner assigner;

    system("clear");
    
    std::cout << "Roles : \n 1. Administrador\n 2. Tutor\n 3. Estudiante\n";
    std::cin >> rol;

    system("clear");

    if(rol ==1){

        do {
            std::cout << "\n\n\n";
            std::cout << "\n================================ MENÚ PRINCIPAL ================================\n\n\n\n";
            std::cout << "\t\t         1. Mostrar estudiantes y tutores\n\n";
            std::cout << "\t\t        2. Asignar tutores automáticamente\n\n";
            std::cout << "\t\t           3. Asignar tutor manualmente\n\n";
            std::cout << "\t\t\t      4. Salir del programa\n\n\n";
            std::cout << "Seleccione una opción: ";
            std::cin >> opcion;




            std::vector<Student> pendientes;

            switch(opcion) {
                case 1:
                    mostrarAsignaciones(students, tutors);
                    std::cout << "\n\nPresione Enter para continuar...";
                    std::cin.get();
                    std::cin.get();
                    system("clear");
                    
                    break;
                case 2:
                    for (const Student& s : students) {
                        if (s.getIdTutor()=="0") pendientes.push_back(s);
                    }

                    assigner.assignTutors(students, tutors, areaMap);
                    writeStudentsCSV("students.csv", students);

                    mostrarNuevosAsignados(pendientes, students, tutors);

                    std::cout << "\n\nPresione Enter para continuar...";
                    std::cin.get();
                    std::cin.get();
                    system("clear");
                    break;

                case 3:
                    AsignacionManual(students, tutors);
                    writeStudentsCSV("students.csv", students);

                    std::cout << "\n\nPresione Enter para continuar...";
                    std::cin.get();
                    std::cin.get();
                    system("clear");
                    break;

                case 4:
                    std::cout << "Saliendo del programa...\n";
                    break;
                default:
                    std::cout << "Opción no válida, intente nuevamente.\n";
            }
        } while(opcion != 4);

    }

    else if (rol ==2 || rol ==3){
        std::string miId;
        if(rol == 2){
            miId = seleccionarTutor(tutors);
        }
        else{
            miId = seleccionarEstudiante(students);
        }
        system("clear");
        do{
            std::cout << "\n\n\n";
            std::cout << "\n================================ MENÚ PRINCIPAL ================================\n\n\n\n";
            std::cout << "\t\t               1. Enviar mensajes \n\n";
            std::cout << "\t\t\t      2. Salir del programa\n\n\n";
            std::cout << "Seleccione una opción: ";
            std::cin >> opcion;

            switch(opcion){
                case 1:
                    iniciarChat(miId, students, tutors);
                    std::cout << "\n\nPresione Enter para continuar...";
                    std::cin.get();
                    std::cin.get();
                    break;
                case 2:
                    std::cout << "Saliendo del programa...\n";
                    break;
                default:
                    std::cout << "Opción no válida, intente nuevamente.\n";
            }

        } while (opcion !=2);
    }



    else {
        std::cout << "Opción no válida, saliendo del programa.\n";
    }

    return 0;
}
