#include "UnixSocketClient.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

bool UnixSocketClient::connectToServer(const std::string& path) {

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) {
        std::cerr << "Failed to create Unix socket\n";
        return false;
    }

    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        path.c_str(),
        sizeof(address.sun_path) - 1
    );

    if (connect(
            fd,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) < 0) {

        std::cerr << "Failed to connect to Game Server\n";
        close(fd);
        fd = -1;
        return false;
    }

    std::cout << "Connected to Game Server\n";
    return true;
}

bool UnixSocketClient::sendMessage(const std::string& message) {

    if (fd < 0) {
        return false;
    }

    ssize_t bytesSent = send(
        fd,
        message.c_str(),
        message.size(),
        0
    );

    return bytesSent == static_cast<ssize_t>(message.size());
}

bool UnixSocketClient::receiveMessage(std::string& message) {

    char buffer[1024];

    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        return false;
    }

    message.assign(buffer, bytesRead);

    return true;
}