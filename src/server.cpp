#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <algorithm>
#include <string>

// Linux socket headers (replaces WinSock2.h)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// On Linux, SOCKET is just an int
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define closesocket(s) close(s)

int main() {
    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
    std::vector<SOCKET> clients;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed\n";
        return 1;
    }

    if (listen(serverSock, 5) == SOCKET_ERROR) {
        std::cout << "Listen failed\n";
        return 1;
    }

    std::cout << "Waiting for client...\n";

    while (true) {
        SOCKET clientSock = accept(serverSock, nullptr, nullptr);
        clients.push_back(clientSock);

        std::thread([clientSock, &clients]() {
            char buffer[1024];

            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

                if (bytes <= 0) {
                    std::cout << "Client disconnected\n";
                    closesocket(clientSock);
                    clients.erase(
                        std::remove(clients.begin(), clients.end(), clientSock),
                        clients.end()
                    );
                    break;
                }

                buffer[bytes] = '\0';
                std::cout << "Client says: " << buffer << "\n";

                // Broadcast with newline so Flutter can split messages cleanly
                std::string msg = std::string(buffer) + "\n";
                for (SOCKET c : clients) {
                    if (send(c, msg.c_str(), msg.length(), 0) == SOCKET_ERROR) {
                        std::cout << "Send failed\n";
                    }
                }
            }
        }).detach();
    }

    closesocket(serverSock);
    return 0;
}
