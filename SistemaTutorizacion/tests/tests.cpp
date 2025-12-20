#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "../student.hpp"
#include "../tutor.hpp"
#include "../AutoAssigner.hpp"
#include "../system.hpp"


#include <map>


void testStudentToCSV() {
    std::vector<std::string> row = {
        "S1", "Ana", "Lopez", "Informatica", "ETSI", "ES;EN", "T1"
    };

    Student s(row);
    auto csv = s.toCSV();

    ASSERT_EQUAL("S1", csv[0]);
    ASSERT_EQUAL("Ana", csv[1]);
    ASSERT_EQUAL("Lopez", csv[2]);
    ASSERT_EQUAL("Informatica", csv[3]);
    ASSERT_EQUAL("ETSI", csv[4]);
    ASSERT_EQUAL("ES;EN", csv[5]);
    ASSERT_EQUAL("T1", csv[6]);
}

void testAutoAssignBestTutor() {
    Student s({ "S1", "Ana", "Lopez", "Informatica", "ETSI", "ES", "0" });
    Tutor t1({ "T1", "Juan", "Lopez", "Software", "ETSI", "ES" });
    Tutor t2({ "T2", "Luis", "Garcia", "Matematicas", "FC", "EN" });

    std::vector<Student> students = { s };
    std::vector<Tutor> tutors = { t1, t2 };

    std::map<std::string, std::vector<std::string>> areaMap = {
        { "Software", { "Informatica" } }
    };

    AutoAssigner assigner;
    assigner.assignTutors(students, tutors, areaMap);

    ASSERT_EQUAL("T1", students[0].getIdTutor());
}

void testAutoAssignDoesNotOverride() {
    Student s({ "S1", "Ana", "Lopez", "Informatica", "ETSI", "ES", "T9" });
    Tutor t1({ "T1", "Juan", "Lopez", "Software", "ETSI", "ES" });

    std::vector<Student> students = { s };
    std::vector<Tutor> tutors = { t1 };
    std::map<std::string, std::vector<std::string>> areaMap;

    AutoAssigner assigner;
    assigner.assignTutors(students, tutors, areaMap);

    ASSERT_EQUAL("T9", students[0].getIdTutor());
}


void testAsignacionManual() {
    std::vector<Student> students = {
        Student({"S1","Ana","Perez","Fisica","Ciencias","Ingles","0"})
    };
    std::vector<Tutor> tutors = {
        Tutor({"T1","Luis","Garcia","Fisica","Ciencias","Ingles"})
    };

    std::istringstream input("1\n1\n");
    std::streambuf* orig = std::cin.rdbuf(input.rdbuf());

    AsignacionManual(students, tutors);

    ASSERT_EQUAL("T1", students[0].getIdTutor());

    std::cin.rdbuf(orig); 
}



void testFormatearIdDestino() {
    Student s1({ "S1", "Ana", "Lopez", "Inf", "ETSI", "ES", "T1" });
    Student s2({ "S2", "Luis", "Perez", "Inf", "ETSI", "ES", "T1" });

    Tutor t1({ "T1", "Juan", "Garcia", "Soft", "ETSI", "ES" });

    std::vector<Student> students = { s1, s2 };
    std::vector<Tutor> tutors = { t1 };


    ASSERT_EQUAL("T1", formatearIdDestino(1, students, tutors, "S1"));


    ASSERT_EQUAL("S1", formatearIdDestino(1, students, tutors, "T1"));
}

void testObtenerEstudiantesDelTutorMultiple() {
    std::vector<Student> students = {
        Student({"S4","Pedro","Lopez","Matematicas","Ciencias","Ingles","T6"}),
        Student({"S5","Ana","Gomez","Matematicas","Ciencias","Frances","T6"}),
        Student({"S6","Luis","Martinez","Fisica","Ciencias","Aleman","T7"})
    };

    std::vector<std::string> ids = obtenerEstudiantesDelTutor("T6", students);
    ASSERT_EQUAL(2, ids.size());
    ASSERT_EQUAL("S4", ids[0]);
    ASSERT_EQUAL("S5", ids[1]);
}



int main() {
    cute::suite s;

    s.push_back(CUTE(testStudentToCSV));
    s.push_back(CUTE(testAutoAssignBestTutor));
    s.push_back(CUTE(testAutoAssignDoesNotOverride));
    s.push_back(CUTE(testAsignacionManual));
    s.push_back(CUTE(testFormatearIdDestino));
    s.push_back(CUTE(testObtenerEstudiantesDelTutorMultiple));

    cute::ide_listener<> listener;
    cute::makeRunner(listener)(s, "Pruebas unitarias del sistema");

    return 0;
}
