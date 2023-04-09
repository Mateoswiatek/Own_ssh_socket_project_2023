cmake_minimum_required(VERSION 3.24)
project(server_remote_file)

set(CMAKE_CXX_STANDARD 17)

add_executable(server_remote_file main.cpp)
target_link_libraries(server_remote_file PRIVATE ws2_32) # aby linkowała się biblioteka