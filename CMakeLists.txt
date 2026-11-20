cmake_minimum_required(VERSION 3.20)
project(P1_read_write C)

set(CMAKE_C_STANDARD 11)

# Sortie des binaires dans bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(philosophes src/part1/philosophes.c)
add_executable(producer_consumer src/part1/producer_consumer.c)
add_executable(reader_writer src/part1/reader_writer.c)
