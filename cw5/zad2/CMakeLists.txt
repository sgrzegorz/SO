cmake_minimum_required(VERSION 2.8)
project(zad2 C)

set(CMAKE_C_STANDARD 99)

add_executable(slave slave.c)
add_executable(master master.c)
add_executable(zad main.c)