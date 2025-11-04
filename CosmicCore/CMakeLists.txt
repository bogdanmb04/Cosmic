cmake_minimum_required(VERSION 3.16)
project(CosmicCore LANGUAGES CXX)

file(GLOB_RECURSE CB_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(CosmicCore STATIC ${CB_SOURCES})

target_include_directories(CosmicCore PRIVATE ${CMAKE_SOURCE_DIR}/CosmicBusiness/src)

target_link_libraries(CosmicCore PRIVATE CosmicBusiness)

target_compile_features(CosmicCore PUBLIC cxx_std_20)

