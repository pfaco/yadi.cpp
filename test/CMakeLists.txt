cmake_minimum_required(VERSION 2.8.3)
cmake_policy(SET CMP0048 NEW)
project(yadi_test
        VERSION 0.0.1)

## Sources
set(yadi_test_SRC
        ../src/data_type.cpp
        ../src/cosem.cpp
        ../src/hdlc.cpp
        ../src/hdlc_frame.cpp
        ../src/logical_name.cpp
        ../src/security.cpp
        catchmain.cpp
        test_dlms_type.cpp
        test_cosem.cpp test_hdlc.cpp)

## Add yadi test target
add_executable(${PROJECT_NAME} ${yadi_test_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ../include)

enable_testing()

add_test(NAME Tests COMMAND yadi_test)
