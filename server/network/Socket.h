#pragma once
#include "SessionManager.h"
#include "UnixSocketClient.h"

class Socket {

public:
    bool create();
    bool bind(unsigned short port);
    void receiveUdpPacket();
    bool receiveGameServerMessage();
    bool connectToGameServer();
    void runEventLoop();
    
private:
    int fd = -1;
    SessionManager sessionManager;
    UnixSocketClient gameServer;
    void runKqueueEventLoop();
    void runEpollEventLoop();
};