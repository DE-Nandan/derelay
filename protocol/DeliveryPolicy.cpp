#include "DeliveryPolicy.h"

DeliveryType getDeliveryType(PacketType type) {

    switch (type) {

        case PacketType::JOIN:
            return DeliveryType::RELIABLE;

        case PacketType::MOVE_UP:
        case PacketType::MOVE_DOWN:
        case PacketType::STATE:
        case PacketType::PING:
            return DeliveryType::UNRELIABLE;

        default:
            return DeliveryType::UNRELIABLE;
    }
}