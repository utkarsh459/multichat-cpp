#include "Client.h"

int main() {
    Client client("127.0.0.1", 8080);
    client.connectToServer();
    client.sendMessage("Hello from client!");
    client.disconnect();
    return 0;
}