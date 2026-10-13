# mainprocess —— 主进程子项目：子线程订阅消息队列 + 主线程心跳
cmake_minimum_required(VERSION 3.10.0)
project(mainprocess LANGUAGES C)

# 可执行文件输出到 bin/（和 configprocess 保持同一惯例，别散在 build 里）
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# glib：读 ini 用
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# add_executable(${PROJECT_NAME} src/main.c)
#批量添加，源文件列表
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     # "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
     # "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)
# 排除构建目录
list(FILTER SOURCES EXCLUDE REGEX "/build/")
#编译可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# pthread：用了 pthread_create / pthread_detach，必须找 Threads 并链上
# （等价于 gcc 的 -pthread；缺了会报 undefined reference to `pthread_create'）
find_package(Threads REQUIRED)
# ---------- CURL ----------
# CMake 自带 CURL 的查找模块
find_package(CURL REQUIRED)
# ---------- cJSON ----------
# CMake不自带 cJSON ,CMake 内置了 FindCURL.cmake要用查找机制
# 思路：优先用 pkg-config（最通用），找不到再手动指定 /usr/local
if(PkgConfig_FOUND)
    pkg_check_modules(CJSON QUIET libcjson)
endif()
# 兜底：直接指定 /usr/local（cJSON 装在这里）
if(NOT CJSON_FOUND)
    set(CJSON_INCLUDE_DIRS /usr/local/include)
    set(CJSON_LIBRARIES    /usr/local/lib/libcjson.so)
endif()
# ---------- Paho MQTT C ----------
# find_path(PAHO_INCLUDE_DIR NAMES MQTTClient.h
#           PATHS /usr/local/include /usr/include)
# find_library(PAHO_MQTT3C_LIBRARY NAMES paho-mqtt3c
#              PATHS /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/inc
    ${CURL_INCLUDE_DIRS}
    ${CJSON_INCLUDE_DIRS}
     ${GLIB_INCLUDE_DIRS}
     # ${PAHO_INCLUDE_DIR}
)
# 链公共库：msgqueue 的封装和 inc 头文件路径都自动到位
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ads_common 
    Threads::Threads
    ${CURL_LIBRARIES}
    ${CJSON_LIBRARIES}
    ${GLIB_LIBRARIES}
#     ${PAHO_MQTT3C_LIBRARY}
)