#include "Client.h"

Client::Client(std::string ip, int port) {
    this->serverIP = ip;
    this->port = port;
    sock = INVALID_SOCKET;
}

void Client::sendMessage(std::string message) {
    if (sock == INVALID_SOCKET) {
        std::cout << "Not connected to server!\n";
        return;
    }

    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent: " << message << "\n";
}

void Client::disconnect() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        WSACleanup();
        std::cout << "Disconnected from server.\n";
    }
}


void Client::connectToServer() {
    // Step 1: Start Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) { //MAKEWORD(2,2) specifies version 2.2 of Winsock
        std::cout << "WSAStartup failed!\n";
        return;
    }

    // Step 2: Create socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//AF_INET for IPv4(AF IS ADDRESSFAMILY, INET IS INTERNET ADDRESS),
    // SOCK_STREAM for TCP, IPPROTO_TCP for TCP protocol(MAKES TCP EXPLICIT)
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed!\n";
        return;
    }


    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);//HTONS converts the port number from host byte order to network byte order (big-endian)
    //WITHOUT HTONS, WRONG PORT NUMBER WILL BE USED AS THE BYTE ORDER IS DIFFERENT BETWEEN HOST AND NETWORK
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
// INET_PTON converts the IP address from text to binary form.
// AF_INET specifies that the address is an IPv4 address,
// serverIP.c_str() CHNAGES OLD C STYLE TEXT
// and &serverAddr.sin_addr is a pointer to the location where the binary form of the IP address will be stored.

if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed!\n";
        return;
    }

    std::cout << "Connected to server!\n";
}