cmake_minimum_required(VERSION 3.10.0)
project(configprocess VERSION 0.1.0 LANGUAGES C)

# 1. 测试与调试配置
set(CMAKE_BUILD_TYPE Debug)   # debug 模式，支持动态调试
include(CTest)
enable_testing()

# 2. 头文件路径：这样代码里就能直接 #include "mongoose.h"
include_directories(${PROJECT_SOURCE_DIR}/inc)

# 3. 可执行文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)   # 可执行文件输出到 bin/
#单个添加，源文件列表
# add_executable(configprocess src/main.c)
#批量添加，源文件列表
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     # "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
     # "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)
# 排除构建目录
list(FILTER SOURCES EXCLUDE REGEX "/build/")
# 编译可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 5. 运行时资源路径：编译期注入绝对路径，避免依赖进程的工作目录
#    mg_http_serve_dir 的 root_dir 与 page404 都是相对 CWD 解析的，
#    用绝对路径后从 build/ 或 systemd 启动都能正常服务静态文件。
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/www" STATIC_ROOT_DIR)
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/www/404.html" STATIC_404_PAGE)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STATIC_ROOT="${STATIC_ROOT_DIR}"
    STAITC_404="${STATIC_404_PAGE}"
)

# 6. 打包配置
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# 5. glib (config_logic 存 ini 用)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLIB_LIBRARIES} ads_common)
