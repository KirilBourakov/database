cmake_minimum_required(VERSION 4.0)
project(database C)

set(CMAKE_C_STANDARD 11)

include_directories(src lib/unity lib/cmock)

# Library for core logic
add_library(db_lib STATIC
    src/table.c
    src/schema.c
)

# Main executable
add_executable(database src/main.c)
target_link_libraries(database db_lib)

# Unity
add_library(unity STATIC lib/unity/unity.c)

# Testing
enable_testing()
add_executable(test_table tests/test_table.c)
target_link_libraries(test_table db_lib unity)
add_test(NAME TableTests COMMAND test_table)
