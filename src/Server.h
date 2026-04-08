#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class Server {
private:
    SOCKET listenSock;
    int port;

public:
    Server(int port);
    void start();
    void acceptClient();
    void stop();
};

#endif