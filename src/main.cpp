#include "Server.h"

int main() {
    Server server(8080);
    server.start();
    server.acceptClient();
    server.stop();
    return 0;
}