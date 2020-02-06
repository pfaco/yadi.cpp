cmake_minimum_required(VERSION 3.10)

project(
        yadi_dlms_test
        
        LANGUAGES
                CXX
)

add_executable(
        yadi_dlms_test 
        
        catchmain.cpp
        src/test_cosem_serializer.cpp

        ../src/cosem_serializer.cpp
)

set_target_properties(
        yadi_dlms_test

        PROPERTIES
                CXX_STANDARD 17
)

target_include_directories(
        yadi_dlms_test

        PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
