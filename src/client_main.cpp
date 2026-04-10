#include "Client.h"
#include <iostream>

int main() {
    Client client("127.0.0.1", 8080);
    client.connectToServer();

    std::string message;

    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (message == "exit") break;

        client.sendMessage(message);
    }

    client.disconnect();
    return 0;
}