#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //fixed typo (scket → socket)

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    //thre we were bindingin server.cpp, here we are connecting to the server

    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    std::thread([&sock]() {
        char buffer[1024];

        while (true) {
            memset(buffer, 0, sizeof(buffer));

            int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);

            if (bytes <= 0) break;

            buffer[bytes] = '\0';

            std::cout << "\nMessage: " << buffer << "\n";
        }
    }).detach();

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);

        if (msg.empty()) continue;

        std::string fullMsg = name + ": " + msg;

        int s = send(sock, fullMsg.c_str(), fullMsg.length(), 0);
        std::cout << "sent: " << s << "\n";
    }//send sends data to the connected socket, in this case, we are sending the message entered by the user to the server

    closesocket(sock);
    WSACleanup();
}