cmake_minimum_required(VERSION 4.3)
project(P1_read_write C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

# Sortie des binaires dans bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(philosophes src/philosophes.c)
add_executable(prodcons src/prodcons.c)
add_executable(read-write src/read-write.c)
