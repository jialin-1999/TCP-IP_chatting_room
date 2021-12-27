cmake_minimum_required(VERSION 3.17)
project(TCPIP)

set(CMAKE_CXX_STANDARD 14)

add_executable(TCPIP main.cpp)
add_executable(client client.cpp)
add_executable(server server.cpp)
add_executable(client2 client2.cpp)