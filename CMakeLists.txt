cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O3)

project(CppArrayVsVector VERSION 0.1
              DESCRIPTION "placeholder"
              LANGUAGES CXX
)

add_executable(c_arrays c_arrays.cpp)
add_executable(std_arrays std_arrays.cpp)
add_executable(std_vectors std_vectors.cpp)