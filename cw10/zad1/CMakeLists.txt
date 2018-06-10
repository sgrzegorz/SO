cmake_minimum_required(VERSION 2.8)
project(Sockets C)


add_compile_options(-std=c11)
add_executable(server common.h server.c)
add_executable(client common.h client.c)
target_link_libraries(server pthread)