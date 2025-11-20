cmake_minimum_required(VERSION 3.16)
project(PacmanGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(BUILD_GUI "Build GUI application" ON)

# Add subdirectories
add_subdirectory(Logic)

if(BUILD_GUI)
    add_subdirectory(GUI)
endif()
