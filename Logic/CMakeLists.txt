cmake_minimum_required(VERSION 3.16)
project(PacmanLogic CXX)

# Source files
set(LOGIC_SOURCES
        Source/GameEngine.cpp
        Source/Ghost.cpp
        Source/Vector2.cpp
)

# Header files
set(LOGIC_HEADERS
        Include/IEventListener.hpp
        Include/IGameEngine.hpp
        Include/GameConfig.hpp
        Include/IGhost.hpp
        Include/GhostModeController.hpp
        Include/Map.hpp
        Include/Direction.hpp
        Include/TileType.hpp
        Include/GameState.hpp
        Include/GhostType.hpp
        Include/GhostMode.hpp
        Include/Vector2.hpp
        Include/PlayerState.hpp
        Include/GhostState.hpp
)

# Create static library
add_library(PacmanLogic STATIC ${LOGIC_SOURCES} ${LOGIC_HEADERS} "Source/Vector2.cpp")

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

# Provide legacy/expected alias so other subprojects can link to Cosmic::Core
add_library(Cosmic::Core ALIAS PacmanLogic)
