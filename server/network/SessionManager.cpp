#include "SessionManager.h"

int SessionManager::createSession(const std::string& ip, int port) {

    int playerId = nextPlayerId++;

    sessions[playerId] = {
        playerId,
        ip,
        port
    };

    return playerId;
}