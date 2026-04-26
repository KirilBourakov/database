cmake_minimum_required(VERSION 4.0)
project(database C)

set(CMAKE_C_STANDARD 11)

include_directories(src lib/unity lib/cmock)

# Library for core logic
add_library(db_lib STATIC
        src/model/row.c
        src/model/schema.c

        src/storage/page.c
        src/storage/disk.c

        src/access/cursor.c
)

# Main executable
add_executable(database src/main.c)
target_link_libraries(database db_lib)

# Unity
add_library(unity STATIC lib/unity/unity.c)

# Testing
enable_testing()

add_executable(unit_tests
    tests/all_tests.c
    tests/test_table.c
    tests/test_page.c
)
target_link_libraries(unit_tests db_lib unity)
add_test(NAME AllTests COMMAND unit_tests)
