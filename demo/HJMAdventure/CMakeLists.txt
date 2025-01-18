cmake_minimum_required(VERSION 3.30.0)
project(HJMAdventure)

set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")