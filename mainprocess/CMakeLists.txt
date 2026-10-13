# mainprocess -- 主进程子项目: 子线程订阅消息队列 + 主线程心跳
cmake_minimum_required(VERSION 3.10.0)
project(mainprocess LANGUAGES C)

# 可执行文件输出到 bin/(和 configprocess 保持同一惯例, 别散在 build 里)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#add_executable(${PROJECT_NAME} src/main.c)
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
# pthread: 用了 pthread_create / pthread_detach, 必须找 Threads 并链上
# (等价于 gcc 的 -pthread; 缺了会报 undefined reference to `pthread_create')
find_package(Threads REQUIRED)

# 链公共库: msgqueue 的封装和 inc 头文件路径都自动到位
target_link_libraries(${PROJECT_NAME} PRIVATE ads_common Threads::Threads)
