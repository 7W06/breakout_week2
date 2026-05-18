cmake_minimum_required(VERSION 3.10)
project(BrickBreaker)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 关键！强制把项目根目录加入头文件搜索路径（解决WSL工作目录问题）
include_directories(${PROJECT_SOURCE_DIR})

# 查找 raylib
find_package(raylib REQUIRED)

# 添加所有源文件（包括cJSON.c）
add_executable(${PROJECT_NAME}
    main.cpp
    Game.cpp
    Ball.cpp
    Paddle.cpp
    Brick.cpp
    Particle.cpp
    PowerUp.cpp
    cJSON.c
)

# 链接 raylib
target_link_libraries(${PROJECT_NAME} raylib)

# 复制JSON文件到build目录（运行时能找到）
configure_file(${PROJECT_SOURCE_DIR}/levels.json ${CMAKE_BINARY_DIR}/levels.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/save.json ${CMAKE_BINARY_DIR}/save.json COPYONLY)