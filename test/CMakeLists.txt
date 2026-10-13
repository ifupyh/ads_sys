# mainprocess —— 主进程子项目：子线程订阅消息队列 + 主线程心跳
cmake_minimum_required(VERSION 3.10.0)
project(test LANGUAGES C)

# 可执行文件输出到 bin/（和 configprocess 保持同一惯例，别散在 build 里）
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# ---------- CURL ----------
# CMake 自带 CURL 的查找模块
find_package(CURL REQUIRED)

# ---------- cJSON ----------
# CMake不自带 cJSON ,CMake 内置了 FindCURL.cmake要用查找机制
# 思路：优先用 pkg-config（最通用），找不到再手动指定 /usr/local
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(CJSON QUIET libcjson)
endif()
 # 兜底：直接指定 /usr/local（cJSON 装在这里）
# if(NOT CJSON_FOUND)
#     set(CJSON_INCLUDE_DIRS /usr/local/include)
#     set(CJSON_LIBRARIES    /usr/local/lib/libcjson.so)
# endif()

# ---------- 目标 ----------
add_executable(${PROJECT_NAME} src/main.c)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/inc
    ${CURL_INCLUDE_DIRS}
    ${CJSON_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CURL_LIBRARIES}
    ${CJSON_LIBRARIES}
)