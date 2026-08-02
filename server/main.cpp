#include <iostream>

#include "network/Socket.h"

int main() {

    std::cout << "DeRelay Server Started\n";

    Socket socket;

    if (!socket.create())
        return 1;

    if (!socket.bind(5000))
        return 1;

    std::cout << "Waiting for packet...\n";

    socket.receive();

    return 0;
}