cmake_minimum_required(VERSION 3.20)
project(P1 C)

set(CMAKE_C_STANDARD 11)

# Options de compilation imposées par le projet
add_compile_options(-Wall -Wextra -O0)

# Bibliothèque de threads POSIX
find_package(Threads REQUIRED)

# Sortie des binaires dans bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Cibles exécutables
add_executable(philosophes src/part1/philosophes.c)
target_link_libraries(philosophes PRIVATE Threads::Threads)

add_executable(producer_consumer src/part1/producer_consumer.c)
target_link_libraries(producer_consumer PRIVATE Threads::Threads)

add_executable(reader_writer src/part1/reader_writer.c)
target_link_libraries(reader_writer PRIVATE Threads::Threads)

enable_testing()
add_test(NAME test_philosophes COMMAND philosophes 2)
add_test(NAME test_prodcons COMMAND producer_consumer 2 2)
add_test(NAME test_readwrite COMMAND reader_writer 2 2)
