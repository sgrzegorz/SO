cmake_minimum_required(VERSION 2.8)
project(zad1 C)

set(CMAKE_C_STANDARD 99)
add_executable(zad zad.c)
find_package (Threads)
target_link_libraries (zad ${CMAKE_THREAD_LIBS_INIT})
