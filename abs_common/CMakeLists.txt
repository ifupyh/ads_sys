# ads_common —— 公共库子项目：System V 消息队列封装
cmake_minimum_required(VERSION 3.10.0)
project(ads_common LANGUAGES C)

# 静态库：编译出 libads_common.a，谁 link 谁
add_library(ads_common STATIC src/msgqueue.c)

# PUBLIC：链接 ads_common 的目标自动获得 inc 头文件路径，不用各自再 include_directories
target_include_directories(ads_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)