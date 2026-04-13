#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"

// 枚举类型定义
enum class PowerUpType {
    LENGTHEN,
    MULTI_BALL,
    SLOW_BALL
};

// 道具结构体
struct PowerUp {
    PowerUpType type;
    Rectangle rect;
    float speed;
    float timer;

    // 构造函数声明
    PowerUp(Vector2 pos, PowerUpType t);
    
    // 成员函数声明
    void Update(float dt);
    void Draw();
    bool CheckCollision(Rectangle paddleRect);
};

#endif