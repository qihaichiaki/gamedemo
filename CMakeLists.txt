cmake_minimum_required(VERSION 3.30.0)
project(GameDemo)

set(CMAKE_CXX_STANDARD_REQUIRED 17)

# CXX相关编译使用utf-8编码
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 设置公共自定义头文件路径
set(PUBLIC_COMMON "${CMAKE_SOURCE_DIR}/Public/common")

# 设置公共三方库路径
set(PUBLIC_THIRDPARTY "${CMAKE_SOURCE_DIR}/Public/thirdparty")

# 子工程添加目录
add_subdirectory(KongDongWuShi)

add_subdirectory(HJMAdventure)