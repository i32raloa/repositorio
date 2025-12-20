#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "student.hpp"
#include "tutor.hpp"
#include <vector>
#include <string>
#include <map>

std::vector<std::vector<std::string>> readCSV(const std::string& filename);
std::map<std::string, std::vector<std::string>> getAreaMap(const std::string& filename);
void writeStudentsCSV(const std::string& filename, const std::vector<Student>& students);
void mostrarAsignaciones(const std::vector<Student>& students, const std::vector<Tutor>& tutors);
void mostrarNuevosAsignados(const std::vector<Student>& pendientes, const std::vector<Student>& todos, const std::vector<Tutor>& tutors);
void AsignacionManual(std::vector<Student>& students, const std::vector<Tutor> tutors);
void iniciarChat(const std::string& miId,const std::vector<Student>& students, const std::vector<Tutor>& tutors);
std::string seleccionarEstudiante(const std::vector<Student>& students);
std::string seleccionarTutor(const std::vector<Tutor>& tutors);
std::string obtenerTutorDelEstudiante(const std::string& idEstudiante, const std::vector<Student>& students);
std::vector<std::string> obtenerEstudiantesDelTutor(const std::string& idTutor, const std::vector<Student>& students);
std::vector<std::string> mostrarChatsDisponibles(const std::string& miId, const std::vector<Student>& students, const std::vector<Tutor>& tutors); 
std::string formatearIdDestino(int numero, const std::vector<Student>& students, const std::vector<Tutor>& tutors, const std::string& miId);

#endif
