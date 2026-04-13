cmake_minimum_required(VERSION 3.10)
project(BrickBreaker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(BrickBreaker
    main.cpp
    Game.cpp
    Ball.cpp
    Brick.cpp
    Paddle.cpp
    Particle.cpp
    PowerUp.cpp        # 我已经帮你加上！！！
)

find_package(raylib REQUIRED)
target_link_libraries(BrickBreaker raylib)