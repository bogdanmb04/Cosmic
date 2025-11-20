cmake_minimum_required(VERSION 3.16)
project(PacmanLogic CXX)

# Source files
set(LOGIC_SOURCES
        Source/GameEngine.cpp
)

# Header files
set(LOGIC_HEADERS
        Include/GameTypes.hpp
        Include/IEventListener.hpp
        Include/IGameEngine.hpp
        Include/GameConfig.hpp
)

# Create static library
add_library(PacmanLogic STATIC ${LOGIC_SOURCES} ${LOGIC_HEADERS})

# Public include directories
target_include_directories(PacmanLogic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Compile features
target_compile_features(PacmanLogic PUBLIC cxx_std_20)

# Compiler warnings
if(MSVC)
    target_compile_options(PacmanLogic PRIVATE /W4 /permissive-)
else()
    target_compile_options(PacmanLogic PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Create alias for cleaner linking
add_library(Pacman::Logic ALIAS PacmanLogic)
