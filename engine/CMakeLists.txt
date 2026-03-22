add_library(gol_engine_lib STATIC
    src/grid.cpp
    src/rle.cpp
    src/text_pattern.cpp
)

target_include_directories(gol_engine_lib PUBLIC include)
target_compile_features(gol_engine_lib PUBLIC cxx_std_17)

# Optional OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(gol_engine_lib PUBLIC OpenMP::OpenMP_CXX)
endif()
