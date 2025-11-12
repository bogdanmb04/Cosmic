cmake_minimum_required(VERSION 3.16)
project(CosmicGUI LANGUAGES CXX)

file(GLOB_RECURSE CB_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(CosmicGUI ${CB_SOURCES})

target_include_directories(CosmicGUI PRIVATE ${CMAKE_SOURCE_DIR}/core/src)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
)
FetchContent_MakeAvailable(SFML)

target_link_libraries(CosmicGUI PRIVATE CosmicCore SFML::Graphics SFML::Audio SFML::Network)

target_compile_features(CosmicGUI PUBLIC cxx_std_20)

