#include "Socket.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#ifdef __APPLE__
#include <sys/event.h>
#elif defined(__linux__)
#include <sys/epoll.h>
#else
#error "Unsupported platform"
#endif

#include "../../protocol/Packet.h"
#include "../../protocol/DeliveryPolicy.h"

using namespace std;

bool Socket::create() {
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        cout << "Failed to create socket\n";
        return false;
    }

    cout << "UDP Socket created. FD = " << fd << std::endl;
    return true;
}

bool Socket::bind(unsigned short port) {
    if (fd < 0) {
        cout << "Socket not created\n";
        return false;
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(fd,
               reinterpret_cast<sockaddr*>(&address),
               sizeof(address)) < 0) {
        cerr << "Bind failed: " << std::strerror(errno) << std::endl;
        return false;
    }

    cout << "Listening on port " << port << std::endl;
    return true;
}

bool Socket::connectToGameServer() {
    return gameServer.connectToServer("/tmp/derelay.sock");
}

static bool dropFirstAck = true;

void Socket::receiveUdpPacket() {

    char buffer[1024];

    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);

    int bytesReceived = recvfrom(
            fd,
            buffer,
            sizeof(buffer),
            0,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientLength);

    if (bytesReceived < 0) {
        cout << "Receive failed\n";
        cout.flush();
        return;
    }

    cout << "Received " << bytesReceived << " bytes\n";
    cout.flush();

    string message(buffer, bytesReceived);

    Packet packet = deserializePacket(message);

    cout << "Received packet\n";

    cout << "From : "
         << inet_ntoa(clientAddress.sin_addr)
         << ":"
         << ntohs(clientAddress.sin_port)
         << "\n";

    cout.flush();

    std::string ip = inet_ntoa(clientAddress.sin_addr);
    int port = ntohs(clientAddress.sin_port);


    int sessionId =
        sessionManager.getSessionId(ip, port);

    if (packet.type == PacketType::JOIN &&
        sessionId == -1) {

        sessionId =
            sessionManager.createSession(ip, port);

        std::cout
            << "Session created: "
            << sessionId
            << " from "
            << ip
            << ":"
            << port
            << "\n";
    }


    if (packet.deliveryType == DeliveryType::RELIABLE &&
        sessionId != -1) {
        
       
        if (sessionManager.hasProcessedSequence(
                sessionId,
                packet.sequenceNumber)) {

            cout
                << "Duplicate reliable packet: "
                << "session=" << sessionId
                << " seq=" << packet.sequenceNumber
                << "\n";


            Packet ackPacket;

            ackPacket.type = PacketType::ACK;

            ackPacket.payload =
                "ACK|" +
                std::to_string(packet.sequenceNumber);

            ackPacket.deliveryType =
                DeliveryType::UNRELIABLE;

            ackPacket.sequenceNumber = 0;

            std::string ackData =
                serializePacket(ackPacket);

            sendto(
                fd,
                ackData.c_str(),
                ackData.size(),
                0,
                reinterpret_cast<sockaddr*>(&clientAddress),
                clientLength
            );

            cout
                << "Re-sent ACK for sequence "
                << packet.sequenceNumber
                << "\n";

            return;
        }

        sessionManager.markSequenceProcessed(
            sessionId,
            packet.sequenceNumber
        );
    }

    switch (packet.type) {

    case PacketType::JOIN: {

        std::cout << "Packet : JOIN\n";

        gameServer.sendMessage(
            std::to_string(sessionId) + "|JOIN"
        );

        break;
    }

    case PacketType::MOVE_UP: {

        std::cout << "Packet : MOVE_UP\n";

        if (sessionId == -1) {
            cout << "Unknown session\n";
            break;
        }

        std::cout
            << sessionId
            << " will move up";

        gameServer.sendMessage(
            std::to_string(sessionId) + "|MOVE_UP"
        );

        break;
    }

    case PacketType::MOVE_DOWN: {

        std::cout << "Packet : MOVE_DOWN\n";

        if (sessionId == -1) {
            cout << "Unknown session\n";
            break;
        }

        std::cout
            << sessionId
            << " will move down";

        gameServer.sendMessage(
            std::to_string(sessionId) + "|MOVE_DOWN"
        );

        break;
    }

    case PacketType::PING: {

        std::cout << "Packet : PING\n";

        break;
    }

    default:

        std::cout << "Packet : UNKNOWN\n";
    }


    if (packet.deliveryType == DeliveryType::RELIABLE &&
        sessionId != -1) {

        Packet ackPacket;

        ackPacket.type = PacketType::ACK;

        ackPacket.payload =
            "ACK|" +
            std::to_string(packet.sequenceNumber);

        ackPacket.deliveryType =
            DeliveryType::UNRELIABLE;

        ackPacket.sequenceNumber = 0;

        std::string ackData =
            serializePacket(ackPacket);

        sendto(
            fd,
            ackData.c_str(),
            ackData.size(),
            0,
            reinterpret_cast<sockaddr*>(&clientAddress),
            clientLength
        );

        std::cout
            << "Sent ACK for sequence "
            << packet.sequenceNumber
            << "\n";
    }

    cout << "Bytes: " << bytesReceived << "\n";
}
  


bool Socket::receiveGameServerMessage() {

    string response;

    ReceiveResult result =
        gameServer.receiveMessage(response);

    if (result == ReceiveResult::CLOSED) {

        cout << "GameServer connection closed\n";
        return false;
    }

    if (result == ReceiveResult::NO_DATA) {

        return true;
    }

    
    cout << "GameServer response: "
         << response
         << "\n";


    size_t firstSeparator =
        response.find('|');

    if (firstSeparator == string::npos) {

        cout << "Invalid GameServer response\n";
        return true;
    }

    int sessionId =
        stoi(response.substr(0, firstSeparator));


    Session session =
        sessionManager.getSession(sessionId);


    string stateMessage =
        response.substr(firstSeparator + 1);

    size_t secondSeparator =
        stateMessage.find('|');

    if (secondSeparator == string::npos) {

        cout << "Invalid STATE response\n";
        return true;
    }

    string stateType =
        stateMessage.substr(0, secondSeparator);

    if (stateType != "STATE") {

        cout << "Unknown GameServer response\n";
        return true;
    }


    string remaining =
        stateMessage.substr(secondSeparator + 1);

    size_t thirdSeparator =
        remaining.find('|');

    if (thirdSeparator == string::npos) {

        cout << "Invalid STATE response\n";
        return true;
    }

    string playerId =
        remaining.substr(0, thirdSeparator);

    string y =
        remaining.substr(thirdSeparator + 1);


    Packet packet;

    packet.type = PacketType::STATE;
    packet.payload = playerId + "|STATE|" + y;

    packet.deliveryType =
        getDeliveryType(packet.type);

    packet.sequenceNumber = 0;

    if (packet.deliveryType == DeliveryType::RELIABLE) {

        packet.sequenceNumber =
            sessionManager.getNextSequenceNumber(sessionId);
    }

    string clientResponse =
        serializePacket(packet);

    sockaddr_in clientAddress{};

    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(session.port);

    inet_pton(
        AF_INET,
        session.ip.c_str(),
        &clientAddress.sin_addr
    );

    sendto(
        fd,
        clientResponse.c_str(),
        clientResponse.size(),
        0,
        reinterpret_cast<sockaddr*>(&clientAddress),
        sizeof(clientAddress)
    );

    cout << "Forwarded Player "
         << playerId
         << " state to Session "
         << sessionId
         << "\n";

    return true;
}

#ifdef __APPLE__

void Socket::runKqueueEventLoop() {

    int kqueueFd = kqueue();

    if (kqueueFd < 0) {

        cerr << "Failed to create kqueue: "
             << strerror(errno)
             << "\n";

        return;
    }

    int gameServerFd =
        gameServer.getFd();

    struct kevent changeList[2];

    EV_SET(
        &changeList[0],
        fd,
        EVFILT_READ,
        EV_ADD,
        0,
        0,
        nullptr);

    EV_SET(
        &changeList[1],
        gameServerFd,
        EVFILT_READ,
        EV_ADD,
        0,
        0,
        nullptr);

    if (kevent(
            kqueueFd,
            changeList,
            2,
            nullptr,
            0,
            nullptr) < 0) {

        cerr << "Failed to register sockets with kqueue: "
             << strerror(errno)
             << "\n";

        close(kqueueFd);

        return;
    }

    cout << "DeRelay kqueue event loop started\n";

    struct kevent events[10];

    while (true) {

        int readyCount =
            kevent(
                kqueueFd,
                nullptr,
                0,
                events,
                10,
                nullptr);

        if (readyCount < 0) {

            if (errno == EINTR)
                continue;

            cerr << "kevent failed: "
                 << strerror(errno)
                 << "\n";

            break;
        }

        for (int i = 0;
             i < readyCount;
             i++) {

            int readyFd =
                static_cast<int>(
                    events[i].ident);

            if (readyFd == fd) {

                receiveUdpPacket();

            } else if (readyFd == gameServerFd) {

                bool success =
                    receiveGameServerMessage();

                if (!success) {

                    struct kevent change{};

                    EV_SET(
                        &change,
                        gameServerFd,
                        EVFILT_READ,
                        EV_DELETE,
                        0,
                        0,
                        nullptr);

                    kevent(
                        kqueueFd,
                        &change,
                        1,
                        nullptr,
                        0,
                        nullptr);

                    cout
                        << "GameServer socket removed from kqueue\n";
                }
            }
        }
    }

    close(kqueueFd);
}

#endif


/*
 * Linux event loop
 */
#ifdef __linux__

void Socket::runEpollEventLoop() {

    int epollFd =
        epoll_create1(0);

    if (epollFd < 0) {

        cerr << "Failed to create epoll: "
             << strerror(errno)
             << "\n";

        return;
    }

    /*
     * Add UDP socket.
     */
    epoll_event udpEvent{};

    udpEvent.events = EPOLLIN;
    udpEvent.data.fd = fd;

    if (epoll_ctl(
            epollFd,
            EPOLL_CTL_ADD,
            fd,
            &udpEvent) < 0) {

        cerr << "Failed to add UDP socket to epoll: "
             << strerror(errno)
             << "\n";

        close(epollFd);

        return;
    }

    /*
     * Add Unix socket connected to Java.
     */
    int gameServerFd =
        gameServer.getFd();

    epoll_event gameServerEvent{};

    gameServerEvent.events = EPOLLIN;
    gameServerEvent.data.fd =
        gameServerFd;

    if (epoll_ctl(
            epollFd,
            EPOLL_CTL_ADD,
            gameServerFd,
            &gameServerEvent) < 0) {

        cerr << "Failed to add GameServer socket to epoll: "
             << strerror(errno)
             << "\n";

        close(epollFd);

        return;
    }

    cout << "DeRelay epoll event loop started\n";

    epoll_event events[10];

    while (true) {

        int readyCount =
            epoll_wait(
                epollFd,
                events,
                10,
                -1);

        if (readyCount < 0) {

            if (errno == EINTR)
                continue;

            cerr << "epoll_wait failed: "
                 << strerror(errno)
                 << "\n";

            break;
        }

        for (int i = 0;
             i < readyCount;
             i++) {

            int readyFd =
                events[i].data.fd;

            if (readyFd == fd) {

                receiveUdpPacket();

            } else if (readyFd == gameServerFd) {

                bool success =
                    receiveGameServerMessage();

                if (!success) {

                    epoll_ctl(
                        epollFd,
                        EPOLL_CTL_DEL,
                        gameServerFd,
                        nullptr);

                    cout
                        << "GameServer socket removed from epoll\n";
                }
            }
        }
    }

    close(epollFd);
}

#endif


/*
 * Platform-independent entry point.
 */
void Socket::runEventLoop() {

#ifdef __APPLE__

    runKqueueEventLoop();

#elif defined(__linux__)

    runEpollEventLoop();

#endif
}
