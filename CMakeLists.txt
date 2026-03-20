cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(invisible-objects VERSION 1.0.0)

# Make sure Geode SDK is installed
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK environment variable not set. Please install the Geode SDK.")
else()
    message(STATUS "Geode SDK found at: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Collect all .cpp files in src/
file(GLOB_RECURSE SOURCES src/*.cpp)

# Build as a shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link against Geode
target_link_libraries(${PROJECT_NAME} geode-sdk)

# Package the mod
setup_geode_mod(${PROJECT_NAME})
