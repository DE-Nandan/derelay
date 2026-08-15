#pragma once

#include "Session.h"
#include <unordered_map>

class SessionManager {
private:
    std::unordered_map<int, Session> sessions;
    int nextPlayerId = 1;
    std::unordered_map<std::string,int> playerLookup;

public:
    int createSession(const std::string& ip, int port);
    int getPlayerId(const std::string &ip,int port);
};