cmake_minimum_required(VERSION 3.16)
project(CosmicBusiness LANGUAGES CXX)

file(GLOB_RECURSE CB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(CosmicBusiness STATIC ${CB_SOURCES})

target_include_directories(CosmicBusiness PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(CosmicBusiness PUBLIC cxx_std_20)

