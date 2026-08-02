#include "Socket.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

bool Socket::create() {
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        std::cout << "Failed to create socket\n";
        return false;
    }

    std::cout << "UDP Socket created. FD = " << fd << std::endl;
    return true;
}

bool Socket::bind(unsigned short port) {
    if (fd < 0) {
        std::cout << "Socket not created\n";
        return false;
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(fd,
               reinterpret_cast<sockaddr*>(&address),
               sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << std::strerror(errno) << std::endl;
        return false;
    }

    std::cout << "Listening on port " << port << std::endl;
    return true;
}


bool Socket::receive() {

    char buffer[1024];

    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);

    int bytesReceived = recvfrom(
            fd,
            buffer,
            sizeof(buffer),
            0,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientLength);

    if (bytesReceived < 0) {
        std::cout << "Receive failed\n";
        return false;
    }

    std::cout << "Received " << bytesReceived << " bytes\n";

    return true;
}