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

# Common GUI sources (library)
set(GUI_COMMON_SOURCES
        Source/GameScreen.cpp
        Source/InputController.cpp
        Source/MenuScreen.cpp
        Source/Application.cpp
)

# Sources that belong only to the executable
set(GUI_EXEC_SOURCES
        Source/Main.cpp
)

# Header files
set(GUI_HEADERS
        Include/GameScreen.hpp
        Include/InputController.hpp
        Include/MenuScreen.hpp
        Include/IMenuListener.hpp
        Include/Application.hpp
)

# Create static library for GUI implementation
add_library(PacmanGUI STATIC ${GUI_COMMON_SOURCES} ${GUI_HEADERS})

# Expose include directories
target_include_directories(PacmanGUI PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Link libraries that GUI depends on
target_link_libraries(PacmanGUI PUBLIC
        Pacman::Logic
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

# Compile features
target_compile_features(PacmanGUI PUBLIC cxx_std_20)

# Create alias for cleaner linking
add_library(Pacman::GUI ALIAS PacmanGUI)

# Create executable that links the GUI library
add_executable(PacmanGame ${GUI_EXEC_SOURCES})

# Set output directory
set_target_properties(PacmanGame PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin
)

# Include directories for executable
target_include_directories(PacmanGame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Link the GUI library (contains the implementation) and other required libs
target_link_libraries(PacmanGame PRIVATE
        PacmanGUI
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
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:sfml-audio> $<TARGET_FILE_DIR:PacmanGame>
        )

        # SFML's audio module requires OpenAL (openal32.dll) at runtime.
        # Copy every dependency that ships with SFML's extlibs folder so the
        # executable can find OpenAL even when launched outside the build tree.
        set(_sfml_source_dir "")
        if(DEFINED sfml_SOURCE_DIR)
                set(_sfml_source_dir "${sfml_SOURCE_DIR}")
        elseif(DEFINED SFML_SOURCE_DIR)
                set(_sfml_source_dir "${SFML_SOURCE_DIR}")
        endif()

        if(_sfml_source_dir)
                if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                        set(_sfml_extlibs_dir "${_sfml_source_dir}/extlibs/bin/x64")
                else()
                        set(_sfml_extlibs_dir "${_sfml_source_dir}/extlibs/bin/x86")
                endif()

                if(EXISTS "${_sfml_extlibs_dir}")
                        add_custom_command(TARGET PacmanGame POST_BUILD
                                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                                        "${_sfml_extlibs_dir}"
                                        $<TARGET_FILE_DIR:PacmanGame>
                        )
                else()
                        message(WARNING "SFML dependency directory not found: ${_sfml_extlibs_dir}")
                endif()
        else()
                message(WARNING "sfml_SOURCE_DIR is not available; cannot copy OpenAL runtime")
        endif()
endif()

# Copy assets
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET PacmanGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:PacmanGame>/Assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            $<TARGET_FILE_DIR:PacmanGame>/Assets
    )
else()
    add_custom_command(TARGET PacmanGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "[NOTICE] Assets directory not found: ${CMAKE_SOURCE_DIR}/assets - skipping copy"
    )
endif()
