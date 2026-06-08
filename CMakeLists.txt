cmake_minimum_required(VERSION 3.15)
project(FileTreeManager LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(FileTreeManager
    main.c    # 成员C：可视化 + 交互
    tree.c    # 成员A：树结构 + 基础操作
    stat.c    # 成员B：递归遍历 + 统计
)
