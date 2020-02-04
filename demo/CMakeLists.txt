cmake_minimum_required(VERSION 3.8)

project(yadi_demo VERSION 0.0.1)

add_executable(
        yadi_demo 

        main.cpp
        ../src/cosem_serializer.cpp
        ../src/cosem_parser.cpp
)

target_include_directories(
        yadi_demo

        PRIVATE
                ../include
)
