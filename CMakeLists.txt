cmake_minimum_required(VERSION 3.22)
project(project3_serv C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O1")
link_libraries(ws2_32)

add_executable(project3_serv main.c start_serv.c start_serv.h structs.h database.c database.h)