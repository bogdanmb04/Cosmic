cmake_minimum_required(VERSION 3.16)
project(PacmanGUI CXX)

# Fetch SFML
include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(SFML)

# Source files
set(GUI_SOURCES
        Source/Main.cpp
        Source/GameScreen.cpp
        Source/InputController.cpp
        Source/MenuScreen.cpp
        Source/Application.cpp
)

# Header files
set(GUI_HEADERS
        Include/GameScreen.hpp
        Include/InputController.hpp
        Include/MenuScreen.hpp
        Include/IMenuListener.hpp
        Include/Application.hpp
)

# Create executable
add_executable(PacmanGame ${GUI_SOURCES} ${GUI_HEADERS})

# Set output directory
set_target_properties(PacmanGame PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin
)

# Include directories
target_include_directories(PacmanGame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Link libraries
target_link_libraries(PacmanGame PRIVATE
        Pacman::Logic
        sfml-graphics
        sfml-window
        sfml-system
)

# Compile features
target_compile_features(PacmanGame PRIVATE cxx_std_20)

# Copy SFML DLLs (Windows only)
if(WIN32)
    add_custom_command(TARGET PacmanGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics> $<TARGET_FILE_DIR:PacmanGame>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-window> $<TARGET_FILE_DIR:PacmanGame>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system> $<TARGET_FILE_DIR:PacmanGame>
    )
endif()

# Copy assets
add_custom_command(TARGET PacmanGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:PacmanGame>/Assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:PacmanGame>/Assets
)
