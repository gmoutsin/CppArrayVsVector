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
add_executable(c_arrays_dr src/c_arrays_drop_result.cpp)
add_executable(std_arrays_dr src/std_arrays_drop_result.cpp)
add_executable(std_vectors_dr src/std_vectors_drop_result.cpp)
add_executable(euler_allocator src/euler_allocator.cpp)
add_executable(euler_preallocator src/euler_preallocator.cpp)

# mkdir -p build && cmake -H. -Bbuild  && cmake --build build && ./build/c_arrays && ./build/std_arrays && ./build/std_vectors && ./build/c_arrays_dr && ./build/std_arrays_dr && ./build/std_vectors_dr && ./build/euler_allocator && ./build/euler_preallocator
