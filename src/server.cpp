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
//here we are creating a socket using the socket function,
//specifying the address family (AF_INET for IPv4),
//the socket type (SOCK_STREAM for TCP),
//and the protocol (0 to choose the default protocol for the given socket type)

    std::vector<SOCKET> clients;
//keeps here al connected client sockets.

    sockaddr_in serverAddr{};
//this is a structure that holds the address and port information for the server we want to connect to.
// We will fill this structure with the appropriate values before connecting to the server.

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//setsockopt is a function that sets options for the socket. In this case,
//SO_REUSEADDR allows the server to bind to an address that is already in use, which can be helpful during development when you might restart the server frequently.
    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
//bind is a function that binds the socket to a specific address and port. Here, we are binding to on port 8080.
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
//this accepts new clients that connect to the server.
        clients.push_back(clientSock);
//adds the new clientsockets to client vector.

        std::thread([clientSock, &clients]() {
//creates new thread for every new client connection, 
//so that the server can handle multiple clients concurrently.
            char buffer[1024];

            while (true) {
                memset(buffer, 0, sizeof(buffer));
//memset is used to clear the buffer before receiving new data,
//ensuring that old data does not interfere with the new message.
                int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
//recv is a function that receives data from the connected client socket. 
//It reads the incoming message into the buffer and returns the number of bytes received.
                if (bytes <= 0) {
                    std::cout << "Client disconnected\n";
                    closesocket(clientSock);
                    clients.erase(
                        std::remove(clients.begin(), clients.end(), clientSock),
                        clients.end()
                    );
//If the client disconnects (recv returns 0 or a negative value), it removes the client from the list and closes the socket.
                    break;
                }//this loop continuously receives messages from the client. 

                buffer[bytes] = '\0';
//converted the received data into string that ends with null pointer               std::cout << "Client says: " << buffer << "\n";
                std::cout << "Client says: " << buffer << "\n";

                // Broadcast with newline so Flutter can split messages cleanly
                std::string msg = std::string(buffer) + "\n";
                for (SOCKET c : clients) {
                    if (send(c, msg.c_str(), msg.length(), 0) == SOCKET_ERROR)
                    //sends the received message to all connected clieents.
                    //parameters used in send() are the client socket to send to,
                    // the message to send, 
                    //the length of the message,
                    // and flags it 0 for no ajeeb sa behavior.(just for exception handling)
                    {
                        std::cout << "Send failed\n";
                    }
                }//loops throguh all the connected clients and sends the received message to all of them, effectively broadcasting the message to all clients.
            }
        }).detach();
    }

    closesocket(serverSock);
    return 0;
}
