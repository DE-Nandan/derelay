#include "Packet.h"

Packet parsePacket(const std::string& message) {

    if (message == "JOIN")
        return {PacketType::JOIN, ""};

    if (message == "UP")
        return {PacketType::MOVE_UP, ""};

    if (message == "DOWN")
        return {PacketType::MOVE_DOWN, ""};

    if (message == "PING")
        return {PacketType::PING, ""};

    return {PacketType::UNKNOWN, message};
}

