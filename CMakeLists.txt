cmake_minimum_required(VERSION 3.0.0)
project(os VERSION 0.1.0 LANGUAGES C)

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.c)
add_executable(os ${SRC_FILES} main.c)
target_include_directories(os PRIVATE includes/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
