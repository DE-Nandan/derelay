#include <cstdint>
#include <iostream>
#include <string>

#include "../protocol/DeliveryPolicy.h"
#include "../protocol/Packet.h"
#include "../server/network/SessionManager.h"

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
        ++failures;
    }
}

void testPacketParsing() {
    Packet join = parsePacket("JOIN");
    expect(join.type == PacketType::JOIN, "JOIN parses as JOIN");
    expect(join.deliveryType == DeliveryType::RELIABLE, "JOIN is reliable");

    Packet move = parsePacket("UP");
    expect(move.type == PacketType::MOVE_UP, "UP parses as MOVE_UP");
    expect(move.deliveryType == DeliveryType::UNRELIABLE, "UP is unreliable");

    Packet unknown = parsePacket("UNKNOWN");
    expect(unknown.type == PacketType::UNKNOWN, "unknown input parses as UNKNOWN");
    expect(unknown.payload == "UNKNOWN", "unknown payload is preserved");
}

void testPacketSerialization() {
    Packet packet{
        PacketType::JOIN,
        "JOIN",
        DeliveryType::RELIABLE,
        0x01020304
    };

    Packet decoded = deserializePacket(serializePacket(packet));
    expect(decoded.type == PacketType::JOIN, "serialized JOIN keeps its packet type");
       expect(decoded.payload.empty(), "recognized command payload remains empty");
    expect(decoded.deliveryType == DeliveryType::RELIABLE,
           "serialized delivery type is preserved");
    expect(decoded.sequenceNumber == 0x01020304,
           "serialized sequence number is preserved");

    Packet malformed = deserializePacket("short");
    expect(malformed.type == PacketType::UNKNOWN,
           "short packet is rejected as UNKNOWN");
}

void testDeliveryPolicy() {
    expect(getDeliveryType(PacketType::JOIN) == DeliveryType::RELIABLE,
           "JOIN delivery policy is reliable");
    expect(getDeliveryType(PacketType::MOVE_UP) == DeliveryType::UNRELIABLE,
           "MOVE_UP delivery policy is unreliable");
    expect(getDeliveryType(PacketType::STATE) == DeliveryType::UNRELIABLE,
           "STATE delivery policy is unreliable");
}

void testSessionsAndSequences() {
    SessionManager manager;
    int sessionId = manager.createSession("127.0.0.1", 50001);

    expect(sessionId == 1, "first session receives ID 1");
    expect(manager.getSessionId("127.0.0.1", 50001) == sessionId,
           "session can be found by IP and port");
    expect(manager.getSessionId("127.0.0.1", 50002) == -1,
           "unknown endpoint has no session");

    expect(manager.getNextSequenceNumber(sessionId) == 1,
           "first sequence number is 1");
    expect(manager.getNextSequenceNumber(sessionId) == 2,
           "sequence numbers increment per session");

    expect(!manager.hasProcessedSequence(sessionId, 42),
           "new sequence is not marked as processed");
    manager.markSequenceProcessed(sessionId, 42);
    expect(manager.hasProcessedSequence(sessionId, 42),
           "processed sequence is detected as duplicate");
    expect(!manager.hasProcessedSequence(sessionId, 43),
           "different sequence is not treated as duplicate");
}

} 

int main() {
    testPacketParsing();
    testPacketSerialization();
    testDeliveryPolicy();
    testSessionsAndSequences();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "All tests passed\n";
    return 0;
}
