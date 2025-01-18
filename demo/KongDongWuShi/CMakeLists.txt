project(KDWSDemo)

file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
set(DIR_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/manager"
)
set(DIR_EASYXINCLUDE "D:/MyApplicationFile/VS/VisualStudio2022/Community/VC/Auxiliary/VS/include")

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")

# 指定EasyX lib
link_directories("D:/MyApplicationFile/VS/VisualStudio2022/Community/VC/Auxiliary/VS/lib/x64")

add_executable(${PROJECT_NAME} ${LIST_SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${DIR_INCLUDE}
    ${DIR_EASYXINCLUDE}
)


# 复制资源文件到输出目录
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")