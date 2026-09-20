#pragma once

#include <string>
#include <cstdint>
#include "PacketType.h"
#include "DeliveryType.h"


struct Packet {

    PacketType type;
    std::string payload;

    DeliveryType deliveryType;
    uint32_t sequenceNumber;

};

Packet parsePacket(const std::string& message);
std::string serializePacket(const Packet& packet);
Packet deserializePacket(const std::string& data);