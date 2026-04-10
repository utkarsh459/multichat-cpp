#include "Server.h"
#include <iostream>
#include <cstring> // for memset

Server::Server(int port) {
    this->port = port;
    listenSock = INVALID_SOCKET;
}

void Server::handleClient(SOCKET clientSock) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSock, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            std::cout << "Client disconnected.\n";
            break;
        }

        std::cout << "Client says: " << buffer << "\n";
    }

    closesocket(clientSock);
}

void Server::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "WSAStartup failed!\n";
        return;
    }

    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cout << "Socket creation failed!\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Bind failed!\n";
        return;
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed!\n";
        return;
    }

    std::cout << "Server started on port " << port << "\n";
}

void Server::acceptClient() {
    std::cout << "Server ready for multiple clients...\n";

    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);

        SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientLen);

        if (clientSock == INVALID_SOCKET) {
            std::cout << "Accept failed!\n";
            continue;
        }

        std::cout << "New client connected!\n";

        std::thread t(&Server::handleClient, this, clientSock);
        t.detach();
    }
}

void Server::stop() {
    closesocket(listenSock);
    WSACleanup();
    std::cout << "Server stopped.\n";
}