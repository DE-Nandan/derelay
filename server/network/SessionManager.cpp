#include "SessionManager.h"

int SessionManager::createSession(const std::string& ip, int port) {

    int sessionId = nextSessionId++;

    sessions[sessionId] = {
        sessionId,
        ip,
        port
    };

    sessionLookup[ip+":"+std::to_string(port)] = sessionId;

    return sessionId;
}

int SessionManager::getPlayerId(const std::string& ip, int port) {

    auto it = sessionLookup.find(ip+":"+std::to_string(port));

    if(it == sessionLookup.end())
    return -1;

    return it->second;
}

Session SessionManager::getSession(int sessionId) {

    auto it = sessions.find(sessionId);

    if (it == sessions.end()) {
        return Session{};
    }

    return it->second;
}



