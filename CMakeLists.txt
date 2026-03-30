cmake_minimum_required(VERSION 3.10)
project(BrickBreaker)

# 明确指定 C++17，兼容 erase-remove 写法
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

set(SOURCES
    main.cpp
    Game.cpp
    Ball.cpp
    Brick.cpp
    Paddle.cpp
)

set(HEADERS
    Game.h
    Particle.h
    Ball.h
    Brick.h
    Paddle.h
)

add_executable(BrickBreaker ${SOURCES} ${HEADERS})
target_link_libraries(BrickBreaker PRIVATE raylib)

if(WIN32)
    target_link_libraries(BrickBreaker PRIVATE opengl32 gdi32 winmm)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(BrickBreaker PRIVATE m pthread dl X11)
endif()