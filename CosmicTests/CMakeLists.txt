cmake_minimum_required(VERSION 3.16)
project(CosmicTests LANGUAGES CXX)

enable_testing()
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0
)

# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(CosmicTests
  test/test_logic.cpp
)

target_include_directories(CosmicTests PRIVATE ${CMAKE_SOURCE_DIR}/CosmicBusiness/src)

target_link_libraries(CosmicTests PRIVATE gtest_main gmock CosmicBusiness)

target_compile_features(CosmicTests PUBLIC cxx_std_20)

include(GoogleTest)
gtest_discover_tests(CosmicTests)

