#pragma once

#include <string>

class UnixSocketClient {
public:
    bool connectToServer(const std::string& path);
    bool sendMessage(const std::string& message);
    bool receiveMessage(std::string& message);
    int getFd() const {
        return fd;
    }

private:
    int fd = -1;
};