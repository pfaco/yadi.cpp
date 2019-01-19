cmake_minimum_required(VERSION 3.8)

project(yadi_demo VERSION 0.0.1)

## Sources
set(yadi_demo_SRC
        main.cpp
        ../src/cosem.cpp
        ../src/emode.cpp
        ../src/hdlc.cpp
        ../src/hdlc_frame.cpp
        ../src/wrapper.cpp
        ../src/security.cpp
        ../src/data_type.cpp
        ../src/logical_name.cpp)

## Find dependencies
find_package(ssp REQUIRED)
find_package(spdlog REQUIRED)

## Add demo target
add_executable(yadi_demo ${yadi_demo_SRC})

## Include headers
target_include_directories(yadi_demo PRIVATE ../include)

## Add necessary libraries
target_link_libraries(yadi_demo PUBLIC ssp)
