# DeRelay

DeRelay is a **C++ networking engine for multiplayer applications**, designed to sit between an application and the underlying UDP transport.

It started as a pet project to understand how online multiplayer networking works at a lower level — including UDP, sessions, packet serialization, event-driven I/O, and reliable delivery.

The goal is to build a reusable networking layer so application code does not need to deal directly with the underlying UDP mechanics.

## Architecture

```text
                 Application
                     |
          +----------+----------+
          |                     |
   Java Client SDK       Java Server SDK
          |                     |
          +----------+----------+
                     |
                  DeRelay
             C++ Networking Engine
                     |
                    UDP
                     |
                  Network
```

For the current multiplayer test application, the server-side path also uses Unix Domain Socket IPC:

```text
Java Game Server
       |
       | Unix Domain Socket
       |
   DeRelay
       |
       | UDP
       |
  Java Client
```

The game itself is primarily a practical environment for exercising and testing the networking engine.

## What DeRelay Handles

DeRelay currently provides:

* Event-driven UDP networking using `kqueue`
* Session creation and session lookup
* IP address + port based session identification
* Custom binary packet format
* Packet serialization and deserialization
* Reliable and unreliable delivery semantics
* Per-session sequence numbers
* ACK-based reliable delivery
* Timeout-based retransmission
* Duplicate packet detection
* Unix Domain Socket IPC
* Stream framing for the server-side connection
* Routing between the network layer and the Java game server

The application-facing Java Client SDK and Java Server SDK are intended to keep these transport-level details out of application code.

## Reliable Delivery

DeRelay supports both reliable and unreliable messages.

For example:

```text
JOIN      -> Reliable
MOVE_UP   -> Unreliable
MOVE_DOWN -> Unreliable
STATE     -> Unreliable
PING      -> Unreliable
```

A reliable packet contains a sequence number.

```text
[ Flags ][ Sequence Number ][ Payload ]
```

The receiver sends an ACK containing the sequence number.

If the ACK is lost, the sender retransmits the **same packet with the same sequence number**.

DeRelay tracks processed sequence numbers per session so a retransmitted packet can be acknowledged without being processed twice.

This provides reliability semantics on top of UDP while retaining the lightweight nature of datagram-based communication.

## Event-Driven Networking

DeRelay uses macOS `kqueue` for event-driven I/O.

The networking process can monitor multiple sources of work without blocking on any single connection:

```text
             kqueue
             /    \
            /      \
       UDP socket   Unix socket
           |             |
       Client data   Game-server data
```

This allows incoming UDP packets and messages from the Java game server to be handled by the same event loop.

## Session Management

Each network client is associated with a DeRelay session.

The current implementation identifies a client using:

```text
IP address + UDP source port
```

A session contains state such as:

* Session ID
* Client address
* Client port
* Next sequence number
* Processed reliable sequence numbers

A retransmitted packet from the same client therefore maps back to the existing session rather than creating a new one.

## Unix Domain Socket IPC

DeRelay communicates with the Java game server through a Unix Domain Socket.

The connection uses `SOCK_STREAM`, so application messages are framed using a newline delimiter.

DeRelay maintains a receive buffer so that both of these cases are handled correctly:

```text
One message
    ↓
"1|STATE|160\n"
```

and:

```text
Partial / combined stream data
    ↓
"1|STATE|160\n2|STATE|160\n"
```

This keeps stream framing separate from the networking engine's UDP packet handling.

## Java Client / Server SDK

The project includes Java-side components that provide the application-facing layer around DeRelay.

The intended separation is:

```text
Application / Game Logic
        |
   Java SDK
        |
   DeRelay
        |
      UDP
```

The SDK layer is responsible for making communication with DeRelay convenient for Java applications, while DeRelay handles the underlying transport concerns.

## Repository Layout

```text
client/
    test_client.cpp
        Minimal UDP test client

protocol/
    Packet.h / Packet.cpp
        Packet model, serialization and deserialization

    PacketType.h
        Supported packet types

    DeliveryType.h
        Reliable / unreliable delivery types

    DeliveryPolicy.h / DeliveryPolicy.cpp
        Delivery policy for packet types

server/
    main.cpp
        DeRelay server entry point

    network/
        Socket.h / Socket.cpp
            UDP socket, kqueue event loop and network receive path

        Session.h / Session.cpp
            Per-client session state

        SessionManager.h / SessionManager.cpp
            Session creation, lookup and sequence tracking

        UnixSocketClient.h / UnixSocketClient.cpp
            Unix Domain Socket communication with the Java server
```

## Building

### Requirements

* macOS or Linux
* C++17 compiler
* `clang++`

### Shell Build

From the repository root:

```bash
./build.sh
```

The build produces:

```text
server/main.out
```

## Running the Networking Engine

Start DeRelay:

```bash
./server/main.out
```

The UDP server listens on port `5000`.

A simple test client can be used to send commands:

```bash
./client/derelay_client
```

Example commands:

```text
JOIN
UP
DOWN
PING
```

The test client communicates with:

```text
127.0.0.1:5000
```

The current project also contains the Java-side server and client components used for end-to-end testing of the networking engine.

## Development Status

### Implemented

* UDP socket creation and binding
* UDP packet reception
* Packet serialization and deserialization
* Binary packet header
* Packet type parsing
* Reliable / unreliable delivery semantics
* Per-session sequence numbers
* ACK handling
* Client-side retransmission
* Duplicate reliable packet detection
* Session creation and lookup
* Event-driven I/O using `kqueue`
* Unix Domain Socket IPC
* Stream framing for `SOCK_STREAM`
* Java-side client/server communication
* End-to-end multiplayer networking flow

### Current Areas of Development

* Heartbeats and session expiration
* Bounded retransmission and failure states
* Rate limiting / congestion handling
* More robust malformed-packet validation
* Fault injection and automated networking tests
* Metrics and observability
* Further SDK packaging and integration improvements

## Design Direction

DeRelay is intentionally being developed as a **networking engine rather than a game-specific networking implementation**.

The multiplayer game provides a concrete environment to exercise the system, but the underlying components are designed around general networking concerns:

```text
Sessions
Packets
Delivery semantics
Sequence numbers
ACKs
Retransmission
Duplicate detection
Event-driven I/O
IPC
```

The longer-term goal is a networking layer that a Java application can plug into through the Client and Server SDKs without needing to understand the underlying UDP implementation.

## Git Hygiene

Generated binaries, debug symbols, object files, and local IDE state should not be committed.

The root `README.md`, source code, protocol definitions, documentation, and project configuration should remain tracked.
