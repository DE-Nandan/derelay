#pragma once

#include <string>

enum class ReceiveResult {
    MESSAGE,
    NO_DATA,
    CLOSED
};

class UnixSocketClient {
public:
    bool connectToServer(const std::string& path);
    bool sendMessage(const std::string& message);
    ReceiveResult receiveMessage(std::string& message);
    int getFd() const {
        return fd;
    }

private:
    int fd = -1;
    std::string receiveBuffer;
};