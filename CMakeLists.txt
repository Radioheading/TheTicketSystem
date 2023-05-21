cmake_minimum_required(VERSION 3.16)
project(TheTicketSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -Ofast")
add_executable(code main.cpp src/user.hpp)