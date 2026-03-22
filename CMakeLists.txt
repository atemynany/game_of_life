cmake_minimum_required(VERSION 3.16)
project(game_of_life LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(engine)

# Tests
option(GOL_BUILD_TESTS "Build tests" ON)
if(GOL_BUILD_TESTS)
    # Fetch Catch2
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(test_grid tests/cpp/test_grid.cpp)
    target_link_libraries(test_grid PRIVATE gol_engine_lib Catch2::Catch2WithMain)

    add_executable(test_rle tests/cpp/test_rle.cpp)
    target_link_libraries(test_rle PRIVATE gol_engine_lib Catch2::Catch2WithMain)

    include(CTest)
    include(Catch)
    catch_discover_tests(test_grid)
    catch_discover_tests(test_rle)
endif()

# Pybind11 bindings
option(GOL_BUILD_BINDINGS "Build Python bindings" OFF)
if(GOL_BUILD_BINDINGS)
    add_subdirectory(bindings)
endif()
