cmake_minimum_required(VERSION 3.13)
project(myasmjit)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
aux_source_directory(myasmjit src)

add_executable(myasmjit main.cpp src/MAsmJit.cpp include/MAsmJit.h)