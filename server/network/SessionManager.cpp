#include "SessionManager.h"

int SessionManager::createSession(const std::string& ip, int port) {

    int playerId = nextPlayerId++;

    sessions[playerId] = {
        playerId,
        ip,
        port
    };

    playerLookup[ip+":"+std::to_string(port)] = playerId;

    return playerId;
}

int SessionManager::getPlayerId(const std::string& ip, int port) {

    auto it = playerLookup.find(ip+":"+std::to_string(port));

    if(it == playerLookup.end())
    return -1;

    return it->second;
}