#pragma once

#include <string>
#include <cstdint>
#include <unordered_set>

struct Session {
    int playerId;
    std::string ip;
    int port;
    uint32_t nextSequenceNumber = 1;

    uint32_t getNextSequenceNumber(int sessionId);
    std::unordered_set<uint32_t> processedSequences;
};