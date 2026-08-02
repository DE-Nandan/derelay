#pragma once

class Socket {

public:
    bool create();
    bool bind(unsigned short port);
    bool receive();
    
private:
    int fd = -1;
};