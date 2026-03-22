find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.12.0
    )
    FetchContent_MakeAvailable(pybind11)
endif()

pybind11_add_module(gol_engine gol_module.cpp)
target_link_libraries(gol_engine PRIVATE gol_engine_lib)

# Alias to avoid name conflict: library target vs module target
# The engine library needs a different target name
