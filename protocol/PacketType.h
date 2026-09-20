#pragma once

enum class PacketType {
    JOIN,
    MOVE_UP,
    MOVE_DOWN,
    PING,
    STATE,
    UNKNOWN
};