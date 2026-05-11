cmake_minimum_required(VERSION 3.10)
project(BrickBreaker)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 找到raylib库
find_package(raylib REQUIRED)

# 收集所有源文件【我帮你补全了所有缺失的文件！】
set(SOURCES
    main.cpp
    Game.cpp
    Ball.cpp
    Paddle.cpp
    Brick.cpp
    Particle.cpp
    PowerUp.cpp
)

# 收集所有头文件【我帮你补全了！】
set(HEADERS
    Game.h
    Ball.h
    Paddle.h
    Brick.h
    Particle.h
    PowerUp.h
)

# 创建可执行文件
add_executable(BrickBreaker ${SOURCES} ${HEADERS})

# 链接raylib库
target_link_libraries(BrickBreaker PRIVATE raylib)

# Windows 平台
if(WIN32)
    target_link_libraries(BrickBreaker PRIVATE opengl32 gdi32 winmm)
endif()

# Linux 平台
if(UNIX AND NOT APPLE)
    target_link_libraries(BrickBreaker PRIVATE m pthread dl rt X11)
endif()