#pragma once

#include "Session.h"
#include <unordered_map>

class SessionManager {
private:
    std::unordered_map<int, Session> sessions;
    int nextPlayerId = 1;

public:
    int createSession(const std::string& ip, int port);
};