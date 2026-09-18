#pragma once

#include <string>

class UnixSocketClient {
public:
    bool connectToServer(const std::string& path);
    bool sendMessage(const std::string& message);

private:
    int fd = -1;
};