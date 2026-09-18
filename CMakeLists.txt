cmake_minimum_required(VERSION 3.20)

project(DeRelay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(derelay_server
    server/main.cpp
    server/network/Socket.cpp
    server/network/SessionManager.cpp
    protocol/Packet.cpp
)

add_executable(derelay_client client/test_client.cpp)
