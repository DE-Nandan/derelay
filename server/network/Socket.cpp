#include "Socket.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

#include "../../protocol/Packet.h"

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


bool Socket::receive() {

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
        return false;
    }

    cout << "Received " << bytesReceived << " bytes\n";
    cout.flush();

   string message(buffer, bytesReceived);

   Packet packet = parsePacket(message);

   cout << "Received packet\n";
   cout << "From : "
              << inet_ntoa(clientAddress.sin_addr)
              << ":"
              << ntohs(clientAddress.sin_port)
              << "\n";
   cout.flush();

    // cout << "Data : " << message << "\n";

    std::string ip = inet_ntoa(clientAddress.sin_addr);
    int port = ntohs(clientAddress.sin_port);

    switch (packet.type) {

    case PacketType::JOIN: {
        std::cout << "Packet : JOIN\n";
        
        int sessionId = sessionManager.createSession(ip, port);

        std::cout << "Session created: " << sessionId
                << " from " << ip << ":" << port << "\n";

        gameServer.sendMessage(
            std::to_string(sessionId) + "|JOIN"
        );

        std::string response;

        if (gameServer.receiveMessage(response)) {

            std::cout << "GameServer response: "
                    << response << "\n";

            sendto(
                fd,
                response.c_str(),
                response.size(),
                0,
                reinterpret_cast<sockaddr*>(&clientAddress),
                clientLength
            );
        }

        break;
    }
    case PacketType::MOVE_UP:{
        std::cout << "Packet : MOVE_UP\n";
        int sessionId = sessionManager.getPlayerId(ip,port);
        if (sessionId == -1) {
        cout << "Unknown session\n";
            break;
        }
        std::cout<<sessionId<<" "<<" will move up";

        gameServer.sendMessage(
            std::to_string(sessionId) + "|MOVE_UP"
        );

        std::string response;


        if (gameServer.receiveMessage(response)) {

            std::cout << "GameServer response: "
                    << response << "\n";

            sendto(
                fd,
                response.c_str(),
                response.size(),
                0,
                reinterpret_cast<sockaddr*>(&clientAddress),
                clientLength
            );
        }

        break;
    }

    case PacketType::MOVE_DOWN:{
        std::cout << "Packet : MOVE_DOWN\n";
        int sessionId = sessionManager.getPlayerId(ip,port);
        if (sessionId == -1) {
        cout << "Unknown session\n";
            break;
        }
        std::cout<<sessionId<<" "<<" will move down";

        gameServer.sendMessage(
            std::to_string(sessionId) + "|MOVE_DOWN"
        );

        std::string response;
        if (gameServer.receiveMessage(response)) {
            sendto(
                fd,
                response.c_str(),
                response.size(),
                0,
                reinterpret_cast<sockaddr*>(&clientAddress),
                clientLength
            );
        }


        break;
    }

    case PacketType::PING:{
        std::cout << "Packet : PING\n";
        break;
    }
    default:
        std::cout << "Packet : UNKNOWN\n";
}


    cout << "Bytes: " << bytesReceived << "\n";



    return true;
}