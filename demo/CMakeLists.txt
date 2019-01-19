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

## Include headers for test target
include_directories(../include)

find_package(ssp REQUIRED)
find_package(spdlog REQUIRED)

## Add demo target
add_executable(yadi_demo ${yadi_demo_SRC})
target_link_libraries(yadi_demo PUBLIC ssp)
target_link_libraries(yadi_demo PRIVATE pthread)
