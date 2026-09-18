#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main() {

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5000);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &serverAddress.sin_addr
    );

    std::string message;

    while (true) {

        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "quit")
            break;

        sendto(
            fd,
            message.c_str(),
            message.size(),
            0,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)
        );
    }

    close(fd);

    return 0;
}