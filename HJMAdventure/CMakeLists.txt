cmake_minimum_required(VERSION 3.30.0)
project(HJMAdventure)

set(HJMASERVER_NAME "HJMAServer")
set(HJMASCLIENT_NAME "HJMAClient")

set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")

file(GLOB_RECURSE SERVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/server/*")
file(GLOB_RECURSE CLIENT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/client/*")

# 启动宽字符支持
add_definitions(-DUNICODE -D_UNICODE)

add_executable(${HJMASERVER_NAME} ${SERVER_SRC})
add_executable(${HJMASCLIENT_NAME} ${CLIENT_SRC})

# 将两个可执行文件添加到构建目标
add_custom_target(${PROJECT_NAME} ALL
    DEPENDS ${HJMASERVER_NAME} ${HJMASCLIENT_NAME}
)

# 包含jsoncpp库
target_include_directories(${HJMASERVER_NAME} PRIVATE "${PUBLIC_THIRDPARTY}/jsoncpp/include")
target_include_directories(${HJMASCLIENT_NAME} PRIVATE "${PUBLIC_THIRDPARTY}/jsoncpp/include")

# 链接jsoncpp库
target_link_libraries(${HJMASERVER_NAME} "${PUBLIC_THIRDPARTY}/jsoncpp/lib/jsoncpp.lib")
target_link_libraries(${HJMASCLIENT_NAME} "${PUBLIC_THIRDPARTY}/jsoncpp/lib/jsoncpp.lib")

# 包含httplib库
target_include_directories(${HJMASERVER_NAME} PRIVATE "${PUBLIC_THIRDPARTY}/cpp-httplib")
target_include_directories(${HJMASCLIENT_NAME} PRIVATE "${PUBLIC_THIRDPARTY}/cpp-httplib")

# client包含easyx库
target_include_directories(${HJMASCLIENT_NAME} PRIVATE "${PUBLIC_THIRDPARTY}/easyx/include")

# client链接easyx库
target_link_libraries(${HJMASCLIENT_NAME}

    # "${PUBLIC_THIRDPARTY}/easyx/lib/EasyXa.lib"  # ansi编码
    "${PUBLIC_THIRDPARTY}/easyx/lib/EasyXw.lib" # unicode编码
)

# client包含common
target_include_directories(${HJMASCLIENT_NAME}
    PRIVATE
    "${PUBLIC_COMMON}"
    "${PUBLIC_COMMON}/easyx"
)

# server包含common
target_include_directories(${HJMASERVER_NAME} PRIVATE "${PUBLIC_COMMON}")

# 复制资源文件到输出目录
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")

# 将 DLL 文件复制到输出目录
add_custom_command(TARGET ${HJMASERVER_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PUBLIC_THIRDPARTY}/jsoncpp/bin/jsoncpp.dll"
    $<TARGET_FILE_DIR:${HJMASERVER_NAME}>
)