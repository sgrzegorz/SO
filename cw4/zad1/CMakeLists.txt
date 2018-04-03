cmake_minimum_required(VERSION 3.9)
project(zad1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -Wall  -fPIC  -ldl")
add_executable(zad1 main.c)
add_executable(clock clock.c)