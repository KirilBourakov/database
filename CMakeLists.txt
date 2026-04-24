cmake_minimum_required(VERSION 4.0)
project(database C)

set(CMAKE_C_STANDARD 11)

add_executable(database
    main.c

    table.h
    table.c
    schema.c
    schema.h

    data_type.h
    utils.h
)
