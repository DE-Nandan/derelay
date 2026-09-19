#pragma once

#include "Session.h"
#include <unordered_map>

class SessionManager {
private:
    std::unordered_map<int, Session> sessions;
    int nextSessionId = 1;
    std::unordered_map<std::string,int> sessionLookup;

public:
    int createSession(const std::string& ip, int port);
    int getPlayerId(const std::string &ip,int port);
    Session getSession(int sessionId);
};