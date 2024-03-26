cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O3)

project(CppArrayVsVector VERSION 1.0
              DESCRIPTION "Array vs Vector benchmark"
              LANGUAGES CXX
)

include_directories(include)
add_executable(c_arrays src/c_arrays.cpp)
add_executable(std_arrays src/std_arrays.cpp)
add_executable(std_vectors src/std_vectors.cpp)