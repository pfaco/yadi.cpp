cmake_minimum_required(VERSION 2.8.3)

# Allow VERSION in the 'project'command
cmake_policy(SET CMP0048 NEW)

project(yadi_dlms
        VERSION 0.0.1
        LANGUAGES CXX)

## Sources
set(yadi_SRCS
    src/cosem.cpp
    src/dlms.cpp
    src/emode.cpp
    src/hdlc.cpp
    src/wrapper.cpp
)

## Add yadi library
add_library(${PROJECT_NAME} STATIC ${yadi_SRCS})

## Include headers
include_directories(include)

## Install
set(INSTALL_LIB_DIR lib)
set(INSTALL_INCLUDE_DIR include)
set(INSTALL_CMAKE_DIR share/serial/cmake)

## Install executable
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${INSTALL_LIB_DIR}
        EXPORT ${PROJECT_NAME}-targets
)

## Install headers
install(FILES   include/cosem.h
                include/dlms.h
                include/hdlc.h
                include/link_layer.h
                include/phy_layer.h
                include/wrapper.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/yadi)

## Install CMake files
install(EXPORT ${PROJECT_NAME}-targets
        DESTINATION ${INSTALL_CMAKE_DIR})

install(FILES   ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake 
                ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR})
