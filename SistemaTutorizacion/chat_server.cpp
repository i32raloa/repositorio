#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <sys/select.h>
#include <sys/stat.h>
#include <ctime>

#define PORT 2000
#define MSG_SIZE 1024
#define MAX_CLIENTS 50

struct Client {
    int socket;
    std::string id;
    bool HistorialEnviado = false;
};

std::vector<Client> clientes;
fd_set readfds;
int server_sd;

void createChatFolder() {
    struct stat st = {0};
    if (stat("chat", &st) == -1) {
        mkdir("chat", 0700);
    }
}


std::string obtenerTimestamp() {
    std::time_t now = std::time(nullptr);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M] ",
                  std::localtime(&now));
    return buffer;
}


void enviarHistorial(int socket,
                     const std::string &id1,
                     const std::string &id2) {
    std::string a = id1 < id2 ? id1 : id2;
    std::string b = id1 < id2 ? id2 : id1;

    std::ifstream file("chat/" + a + b + ".txt");
    if (!file.is_open()) return;

    std::string header = "\n----- HISTORIAL DEL CHAT -----\n";
    send(socket, header.c_str(), header.size(), 0);

    std::string line;
    while (std::getline(file, line)) {
        line += "\n";
        send(socket, line.c_str(), line.size(), 0);
    }

    std::string footer = "-----------------------------\n";
    send(socket, footer.c_str(), footer.size(), 0);
}



void salirCliente(int socket) {
    close(socket);
    FD_CLR(socket, &readfds);
    clientes.erase(std::remove_if(clientes.begin(), clientes.end(),
                                  [socket](const Client &c){ return c.socket == socket; }),
                   clientes.end());
    std::cout << "Cliente <" << socket << "> desconectado\n";
}

void manejador(int signum) {
    std::cout << "\nSe recibió SIGINT. Cerrando servidor...\n";
    for (auto &c : clientes) {
        std::string msg = "Servidor desconectado\n";
        send(c.socket, msg.c_str(), msg.size(), 0);
        close(c.socket);
    }
    close(server_sd);
    exit(0);
}

void guardarMensaje(const std::string &id1, const std::string &id2, const std::string &mensaje) {
    std::string idA = id1 < id2 ? id1 : id2;
    std::string idB = id1 < id2 ? id2 : id1;

    std::string filename = "chat/" + idA + idB + ".txt";
    std::ofstream file(filename, std::ios::app);
    file << mensaje << "\n";
    file.close();
}

int main() {
    createChatFolder();

    server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd == -1) { perror("No se puede abrir el socket"); return 1; }

    int opt = 1;
    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error en bind"); return 1;
    }
    if (listen(server_sd, 5) == -1) {
        perror("Error en listen"); return 1;
    }

    std::cout << "Servidor chat iniciado en puerto " << PORT << "...\n";

    FD_ZERO(&readfds);
    FD_SET(server_sd, &readfds);
    FD_SET(0, &readfds);
    int max_sd = server_sd;

    signal(SIGINT, manejador);

    while (true) {
        fd_set auxfds = readfds;
        int activity = select(FD_SETSIZE, &auxfds, nullptr, nullptr, nullptr);
        if (activity < 0) continue;

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &auxfds)) {
                if (i == server_sd) {
                   
                    sockaddr_in client_addr{};
                    socklen_t addrlen = sizeof(client_addr);
                    int new_sd = accept(server_sd, (struct sockaddr*)&client_addr, &addrlen);
                    if (new_sd == -1) { perror("Error accept"); continue; }

                    if ((int)clientes.size() < MAX_CLIENTS) {
                      
                        char buffer[MSG_SIZE];
                        int bytes = recv(new_sd, buffer, sizeof(buffer)-1, 0);
                        buffer[bytes] = '\0';
                        std::string idCliente(buffer);

                        clientes.push_back({new_sd, idCliente});
                        FD_SET(new_sd, &readfds);
                        if (new_sd > max_sd) max_sd = new_sd;

                        std::cout << "Cliente conectado: " << idCliente << "\n";

                    } else {
                        std::string msg = "Demasiados clientes conectados\n";
                        send(new_sd, msg.c_str(), msg.size(), 0);
                        close(new_sd);
                    }

                } else if (i == 0) {
                    std::string input;
                    std::getline(std::cin, input);
                    if (input == "SALIR") {
                        manejador(SIGINT);
                    }
                } else {
                    char buffer[MSG_SIZE];
                    int bytes = recv(i, buffer, sizeof(buffer)-1, 0);
                    if (bytes <= 0) {
                        salirCliente(i);
                    } else {
                        buffer[bytes] = '\0';
                        std::string data(buffer); 

                        size_t sep = data.find('|');
                        if (sep != std::string::npos) {
                            std::string idDest = data.substr(0, sep);
                            std::string mensaje = data.substr(sep+1);


                            auto it = std::find_if(clientes.begin(), clientes.end(),
                                [i](const Client &c){ return c.socket == i; });
                            std::string idEmisor = (it != clientes.end()) ? it->id : "Unknown";

                            std::string timestamp = obtenerTimestamp();
                            std::string msgCompleto =
                                timestamp + it->id + ": " + mensaje;

                            if (!it->HistorialEnviado) {
                                enviarHistorial(i, it->id, idDest);
                                it->HistorialEnviado = true;
                                continue;
                            }
                          
                            guardarMensaje(idDest, idEmisor, msgCompleto);

                            for (auto &c : clientes) {
                                if (c.id == idDest) {
                                    send(c.socket, msgCompleto.c_str(), msgCompleto.size(), 0);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    close(server_sd);
    return 0;
}
