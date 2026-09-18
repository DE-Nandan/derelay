#pragma once
#include "SessionManager.h"
#include "UnixSocketClient.h"

class Socket {

public:
    bool create();
    bool bind(unsigned short port);
    bool receive();
    bool connectToGameServer();
    
private:
    int fd = -1;
    SessionManager sessionManager;
    UnixSocketClient gameServer;
};