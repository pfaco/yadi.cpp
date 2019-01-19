cmake_minimum_required(VERSION 3.0.0)
project(yadi
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
    src/hdlc_frame.cpp
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

## Install library
install(TARGETS
            ${PROJECT_NAME}
        EXPORT
            ${PROJECT_NAME}
        DESTINATION
            lib/yadi)

## Install headers
install(FILES
            include/yadi/cosem.h
            include/yadi/dlms.h
            include/yadi/emode.h
            include/yadi/hdlc.h
            include/yadi/parser.h
            include/yadi/wrapper.h
        DESTINATION
            include/yadi)

## Install CMake files
install(EXPORT
            ${PROJECT_NAME}
        DESTINATION
            lib/cmake/yadi)

install(FILES
            ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake
        DESTINATION
            lib/cmake/yadi)
