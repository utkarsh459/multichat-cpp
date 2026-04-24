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

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); 
//here we are creating a socket using the socket function,
//specifying the address family (AF_INET for IPv4),
//the socket type (SOCK_STREAM for TCP),
//and the protocol (0 to choose the default protocol for the given socket type)

    sockaddr_in serverAddr{};
//this is a structure that holds the address and port information for the server we want to connect to.
// We will fill this structure with the appropriate values before connecting to the server.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
//inet_pton is a function that converts an IP address from its text representation (in this case, "127.0.0.1") to its binary form 

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    //my sock which is socket where to connect is the ip stored in serverAddr.
    //thre we were bindingin server.cpp, here we are connecting to the server

    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    std::thread([&sock]()
//creates a new thread that will handle receiving messages from the server.
    {
        char buffer[1024];

        while (true) {
            memset(buffer, 0, sizeof(buffer));
//memset is used to clear the buffer before receiving new data,
// ensuring that old data does not interfere with the new message.

            int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
//recv is a function that receives data from the connected socket.

            if (bytes <= 0) break;
//condition to check if the connection has been closed.

            buffer[bytes] = '\0';
//converted the received data into string that ends with null pointer
            std::cout << "\nMessage: " << buffer << "\n";
        }
    }).detach();//detach allows the thread to run independently.
//if i dont write detach(),
//the main thread will wait for this thread to finish before it can continue executing.
//we dont want the main thread to wait for the receiving thread, we want it to run concurrently, so we detach it.
//the main thread is there to send messages, and the receiving thread is there to receive messages from the server at the same time.    
while (true) {
        std::string msg;
//created a string variable msg to hold the message that the user will input.
        std::getline(std::cin, msg);
//getline is a function that reads a line of input from the user and stores it in the msg variable.

        if (msg.empty()) continue;//exception handling 
        std::string fullMsg = name + ": " + msg;

        int s = send(sock, fullMsg.c_str(), fullMsg.length(), 0);
        //send is a function that sends data to the connected socket.
        std::cout << "sent: " << s << "\n";
    }//we are sending the message entered by the user to the server

    closesocket(sock);
    //closesocket is a function that closes the socket connection when we are done.
    //if we dont close the socket, it will remain open and consume system resources.
    WSACleanup();
    //WSACleanup is a function that cleans up the Winsock library when we are done using it.
    //in winsock library we clear resources like sockets and other things that we used during the program.
}