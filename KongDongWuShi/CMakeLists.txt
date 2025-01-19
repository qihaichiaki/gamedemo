project(KongDongWuShi)

file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
set(DIR_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/manager"
    "${PUBLIC_COMMON}"
    "${PUBLIC_COMMON}/easyx"
)
set(DIR_EASYXINCLUDE "${PUBLIC_THIRDPARTY}/EasyX/include")

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")

# 链接EasyX lib
link_directories("${PUBLIC_THIRDPARTY}/EasyX/lib")

add_executable(${PROJECT_NAME} ${LIST_SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${DIR_INCLUDE}
    ${DIR_EASYXINCLUDE}
)

# 复制资源文件到输出目录
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")