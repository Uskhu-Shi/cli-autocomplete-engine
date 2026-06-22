cmake_minimum_required(VERSION 3.15)
project(AutocompleteEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(autocomplete ${SOURCES})

if(MSVC)
    target_compile_options(autocomplete PRIVATE /W4)
else()
    target_compile_options(autocomplete PRIVATE -Wall -Wextra)
endif()