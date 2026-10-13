cmake_minimum_required(VERSION 3.10.0)   # 要求的最低 CMake 版本
project(ads_sys VERSION 0.1.0 LANGUAGES C)  # 父工程名，语言 C
add_subdirectory(abs_common)
# 添加子模块：这一行让 CMake 进入 configprocess 目录去读它的 CMakeLists.txt

add_subdirectory(configprocess)
# 以后新增模块就再加一行，例如：
add_subdirectory(mainprocess)
add_subdirectory(test)
# add_subdirectory(updateprocess)