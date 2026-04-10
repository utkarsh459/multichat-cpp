#include "Server.h"
#include <iostream>

Server::Server(int port) {
    this->port = port;
    listenSock = INVALID_SOCKET;
}

void Server::start() {
    // winsock initialize karo
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "WSAStartup failed!\n";
        return;
    }

    // socket banao
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cout << "Socket creation failed!\n";
        return;
    }

    //Bind bind bind
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Bind failed!\n";
        return;
    }

    // sunao sunao
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed!\n";
        return;
    }

    std::cout << "Server started on port " << port << "\n";
}

void Server::acceptClient() {
    std::cout << "Waiting for client...\n";

    sockaddr_in clientAddr{};
    int clientLen = sizeof(clientAddr);

    SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientLen);

    if (clientSock == INVALID_SOCKET) {
        std::cout << "Accept failed!\n";
        return;
    }

    std::cout << "Client connected!\n";
    closesocket(clientSock);
}

void Server::stop() {
    closesocket(listenSock);
    WSACleanup();
    std::cout << "Server stopped.\n";
}