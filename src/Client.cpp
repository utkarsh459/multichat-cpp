#include "Client.h"

Client::Client(std::string ip, int port) {
    this->serverIP = ip;
    this->port = port;
    sock = INVALID_SOCKET;
}

void Client::connectToServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "WSAStartup failed!\n";
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed!\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed!\n";
        return;
    }

    std::cout << "Connected to server!\n";
}

void Client::sendMessage(std::string message) {
    send(sock, message.c_str(), message.length(), 0);
}

void Client::disconnect() {
    closesocket(sock);
    WSACleanup();
    std::cout << "Disconnected.\n";
}