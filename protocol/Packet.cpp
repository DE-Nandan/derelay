#include "Packet.h"

Packet parsePacket(const std::string& message) {

    if (message == "JOIN")
        return {
            PacketType::JOIN,
            "",
            DeliveryType::RELIABLE,
            0
        };

    if (message == "UP")
        return {
            PacketType::MOVE_UP,
            "",
            DeliveryType::UNRELIABLE,
            0
        };

    if (message == "DOWN")
        return {
            PacketType::MOVE_DOWN,
            "",
            DeliveryType::UNRELIABLE,
            0
        };

    if (message == "PING")
        return {
            PacketType::PING,
            "",
            DeliveryType::UNRELIABLE,
            0
        };

    return {
        PacketType::UNKNOWN,
        message,
        DeliveryType::UNRELIABLE,
        0
    };
}


std::string serializePacket(const Packet& packet) {

    std::string data;

    // Convert internal DeliveryType to wire flag.
    uint8_t flags =
        packet.deliveryType == DeliveryType::RELIABLE
        ? 1
        : 0;

    // 1 byte flags
    data.push_back(
        static_cast<char>(flags)
    );

    // 4 byte sequence number
    uint32_t sequence =
        packet.sequenceNumber;

    for (int i = 0; i < 4; i++) {

        data.push_back(
            static_cast<char>(
                (sequence >> (i * 8)) & 0xFF
            )
        );
    }

    // Application payload
    data += packet.payload;

    return data;
}


Packet deserializePacket(const std::string& data) {

    if (data.size() < 5) {

        return {
            PacketType::UNKNOWN,
            "",
            DeliveryType::UNRELIABLE,
            0
        };
    }

    // Read wire flag
    uint8_t flags =
        static_cast<uint8_t>(data[0]);

    DeliveryType deliveryType =
        flags == 1
        ? DeliveryType::RELIABLE
        : DeliveryType::UNRELIABLE;

    // Read sequence number
    uint32_t sequence = 0;

    for (int i = 0; i < 4; i++) {

        sequence |=
            static_cast<uint32_t>(
                static_cast<unsigned char>(data[i + 1])
            ) << (i * 8);
    }

    // Remaining bytes = application payload
    std::string payload =
        data.substr(5);

    Packet packet =
        parsePacket(payload);

    packet.deliveryType = deliveryType;
    packet.sequenceNumber = sequence;

    return packet;
}