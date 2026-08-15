#pragma once

#include <string>
#include "PacketType.h"

struct Packet {

    PacketType type;
    std::string payload;

};

Packet parsePacket(const std::string& message);