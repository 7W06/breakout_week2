cmake_minimum_required(VERSION 3.10)
project(BrickBreaker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

include_directories(.)

add_executable(BrickBreaker
    main.cpp
    Game.cpp
    Ball.cpp
    Brick.cpp
    Paddle.cpp
)

add_executable(Tests
    test.cpp
    Ball.cpp
    Brick.cpp
)

target_link_libraries(BrickBreaker PRIVATE raylib)
target_link_libraries(Tests PRIVATE raylib)