cmake_minimum_required(VERSION 2.8)
project(Threads1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -Wall") #-Wpedantic
add_executable(zad zad.c )

target_link_libraries(zad m)
find_package (Threads)
target_link_libraries (zad ${CMAKE_THREAD_LIBS_INIT})


