cmake_minimum_required(VERSION 3.9)
project(zad3 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -Wall  ")

add_executable(zad main.c)
add_executable(test1 test1.c)
add_executable(test2 test2.c)