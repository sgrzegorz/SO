cmake_minimum_required (VERSION 3.6)
project (myproject)

set (CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -Wall  -fPIC  -ldl") #-Wpedantic
add_library (static STATIC array.c array.h)
add_library (shared SHARED array.c array.h)
add_executable(pstatic main.c)
add_executable(pshared main.c)
add_executable(pdynamic main.c)

target_compile_definitions(pdynamic PRIVATE DYNAMIC)
target_link_libraries(pstatic static)
target_link_libraries(pshared shared)
target_link_libraries(pdynamic ${CMAKE_DL_LIBS})

#set_target_properties(pdynamic PROPERTIES COMPILE_FLAGS -O2) #-O -O1 -O2 -O3 -O0 -Os -Ofast -Og


