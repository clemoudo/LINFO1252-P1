cmake_minimum_required(VERSION 4.3)
project(P1_read_write C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

# Sortie des binaires dans bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(P1_read_write
        src/philosophes.c
        src/prodcons.c
        src/read-write.c
        include/utils.h)
