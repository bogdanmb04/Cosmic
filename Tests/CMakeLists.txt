cmake_minimum_required(VERSION 3.16)
project(CosmicTests LANGUAGES CXX)

enable_testing()
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.13.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(CosmicTests
        Source/ApplicationTest.cpp
        Source/GameConfigTest.cpp
        Source/GameScreenTest.cpp
        Source/GameTypesTest.cpp
        Source/InputControllerTest.cpp
)

add_library(CosmicTestsLib INTERFACE)
add_library(Cosmic::Tests ALIAS CosmicTestsLib)

target_include_directories(CosmicTestsLib INTERFACE
        ${CMAKE_SOURCE_DIR}/core/src
        ${CMAKE_SOURCE_DIR}/GUI/Include
        ${CMAKE_SOURCE_DIR}/Logic/Include
)

target_link_libraries(CosmicTests PRIVATE CosmicTestsLib)

target_link_libraries(CosmicTests PRIVATE gtest_main gmock Cosmic::Core Pacman::GUI
        sfml-graphics
        sfml-window
        sfml-system
)

target_compile_features(CosmicTests PUBLIC cxx_std_20)

if(WIN32)
        add_custom_command(TARGET CosmicTests POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:sfml-graphics> $<TARGET_FILE_DIR:CosmicTests>
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:sfml-window> $<TARGET_FILE_DIR:CosmicTests>
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:sfml-system> $<TARGET_FILE_DIR:CosmicTests>
        )

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
                        add_custom_command(TARGET CosmicTests POST_BUILD
                                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                                        "${_sfml_extlibs_dir}"
                                        $<TARGET_FILE_DIR:CosmicTests>
                        )
                else()
                        message(WARNING "SFML dependency directory not found: ${_sfml_extlibs_dir}")
                endif()
        else()
                message(WARNING "sfml_SOURCE_DIR is not available; cannot copy OpenAL runtime")
        endif()
endif()

include(GoogleTest)
gtest_discover_tests(CosmicTests)