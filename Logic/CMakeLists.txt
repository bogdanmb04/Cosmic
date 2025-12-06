cmake_minimum_required(VERSION 3.16)
project(PacmanLogic CXX)

# Source files
set(LOGIC_SOURCES
        Source/GameEngine.cpp
        Source/Ghost.cpp
        Source/GhostModeController.cpp
        Source/Map.cpp
        Source/Vector2.cpp
        Source/GhostStrategies/RedStrategy.cpp
        Source/GhostStrategies/PinkStrategy.cpp
        Source/GhostStrategies/BlueStrategy.cpp
        Source/GhostStrategies/OrangeStrategy.cpp
)

# Header files
set(LOGIC_HEADERS
        Include/GameConfig.hpp
        Include/GameEngine.hpp
        Include/GhostModeController.hpp
        Include/IEventListener.hpp
        Include/IGameEngine.hpp
        Include/IGhost.hpp
        Include/Map.hpp
        Include/GameTypes/Direction.hpp
        Include/GameTypes/GameState.hpp
        Include/GameTypes/GhostMode.hpp
        Include/GameTypes/GhostState.hpp
        Include/GameTypes/GhostType.hpp
        Include/GameTypes/PlayerState.hpp
        Include/GameTypes/TileType.hpp
        Include/GameTypes/TileUpdate.hpp
        Include/GameTypes/Vector2.hpp
        Include/GhostStrategies/IGhostStrategy.hpp
        Include/GhostStrategies/RedStrategy.hpp
        Include/GhostStrategies/PinkStrategy.hpp
        Include/GhostStrategies/BlueStrategy.hpp
        Include/GhostStrategies/OrangeStrategy.hpp
)

# Create static library
add_library(PacmanLogic STATIC ${LOGIC_SOURCES} ${LOGIC_HEADERS})

# Public include directories
target_include_directories(PacmanLogic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/GhostStrategies
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/GameTypes
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
