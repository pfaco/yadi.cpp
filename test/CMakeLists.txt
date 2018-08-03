cmake_minimum_required(VERSION 2.8.3)
cmake_policy(SET CMP0048 NEW)
project(yadi_dlms_test
        VERSION 0.0.1)

## Sources
set(yadi_test_SRC
        ../src/data_type.cpp
        catchmain.cpp
        test_dlms_type.cpp)

include_directories(../include)

## Add yadi test target
add_executable(${PROJECT_NAME} ${yadi_test_SRC})
