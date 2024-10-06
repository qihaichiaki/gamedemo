cmake_minimum_required(VERSION 3.30.0)
project(KDWSDemo)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")

file(GLOB_RECURSE LIST_SRC "${CMAKE_SOURCE_DIR}/src/*")
set(DIR_INCLUDE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/common"
    "${CMAKE_SOURCE_DIR}/src/manager"
)
set(DIR_EASYXINCLUDE "D:/MyApplicationFile/VS/VisualStudio2022/Community/VC/Auxiliary/VS/include")

# 指定EasyX lib
link_directories("D:/MyApplicationFile/VS/VisualStudio2022/Community/VC/Auxiliary/VS/lib/x64")

add_executable(${PROJECT_NAME} ${LIST_SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${DIR_INCLUDE}
    ${DIR_EASYXINCLUDE}
)