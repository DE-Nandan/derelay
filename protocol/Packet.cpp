#include "Packet.h"

Packet parsePacket(const std::string& message) {

    if (message == "JOIN\n")
        return {PacketType::JOIN, ""};

    if (message == "UP\n")
        return {PacketType::MOVE_UP, ""};

    if (message == "DOWN\n")
        return {PacketType::MOVE_DOWN, ""};

    if (message == "PING\n")
        return {PacketType::PING, ""};

    return {PacketType::UNKNOWN, message};
}