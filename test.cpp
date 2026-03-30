#include "Ball.h"
#include "Brick.h"
#include <cassert>
#include <iostream>

int main() {
    // 测试1：小球在砖块上方 → 碰撞
    Ball b1({100, 50}, {0, 2}, 10);
    Brick br1(90, 60, 20, 20, 0);
    assert(b1.CheckBrickCollision(br1) == true);
    std::cout << "Test1: PASS\n";

    // 测试2：小球远离 → 不碰撞
    Ball b2({0,0}, {0,0}, 10);
    Brick br2(200,200,20,20,0);
    assert(b1.CheckBrickCollision(br1) == false);
    std::cout << "Test2: PASS\n";

    std::cout << "All tests passed!\n";
    return 0;
}