cmake_minimum_required(VERSION 3.10)
project(BrickBreaker)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 找到raylib库（确保已安装raylib）
find_package(raylib REQUIRED)

# 收集所有源文件
set(SOURCES
    main.cpp
    Ball.cpp
    Paddle.cpp
    Brick.cpp
)

# 收集所有头文件
set(HEADERS
    Ball.h
    Paddle.h
    Brick.h
)

# 创建可执行文件
add_executable(BrickBreaker ${SOURCES} ${HEADERS})

# 链接raylib库
target_link_libraries(BrickBreaker PRIVATE raylib)

# 针对不同平台的额外设置
if(WIN32)
    # Windows下链接额外库
    target_link_libraries(BrickBreaker PRIVATE opengl32 gdi32 winmm)
endif()

if(UNIX AND NOT APPLE)
    # Linux下链接额外库
    target_link_libraries(BrickBreaker PRIVATE m pthread dl rt X11)
endif()