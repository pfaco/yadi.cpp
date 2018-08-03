cmake_minimum_required(VERSION 3.0.0)
project(yadi_dlms
        VERSION 0.0.1
        LANGUAGES CXX)

add_subdirectory(demo)
add_subdirectory(test)

## Sources
set(yadi_SRC
    src/cosem.cpp
    src/data_type.cpp
    src/emode.cpp
    src/hdlc.cpp
    src/logical_name.cpp
    src/security.cpp
    src/wrapper.cpp)

## Add yadi library
add_library(${PROJECT_NAME} STATIC ${yadi_SRC})

## Include headers
target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
