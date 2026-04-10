#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class Client {
private:
    SOCKET sock;
    std::string serverIP;
    int port;

public:
    Client(std::string ip, int port);
    void connectToServer();
    void sendMessage(std::string message);
    void disconnect();
};

#endif