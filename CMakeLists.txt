cmake_minimum_required(VERSION 3.0.0)
project(yadi_dlms
        VERSION 0.0.1
        LANGUAGES CXX)

add_subdirectory(test)
add_subdirectory(demo)

## Sources
set(yadi_SRC
    src/cosem.cpp
    src/dlms.cpp
    src/emode.cpp
    src/hdlc.cpp
    src/data_type.cpp
    src/wrapper.cpp
    src/security.cpp)

## Add yadi library
add_library(${PROJECT_NAME} STATIC ${yadi_SRC})

target_link_libraries(yadi_dlms PUBLIC ssp)

## Include headers
target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
