cmake_minimum_required(VERSION 2.8)
project(zad1 C)

set(CMAKE_C_STANDARD 99)
add_executable(creator creator.c shared_memory.c shared_memory.h)
add_executable(barber barber.c shared_memory.c shared_memory.h)
find_package (Threads)
target_link_libraries (barber ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (creator ${CMAKE_THREAD_LIBS_INIT})