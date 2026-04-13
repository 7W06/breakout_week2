#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"
#include <string>

enum class PowerUpType {
    NONE,
    LENGTHEN_PADDLE,
    MULTI_BALL,
    SLOW_BALL
};

struct PowerUp {
    PowerUpType type;
    Vector2 position;
    float speed;
    float duration;   // 0表示瞬时效果
    Color color;
    float width;
    float height;
    float speedMultiplier; // 仅减速球用

    PowerUp(PowerUpType t, Vector2 pos, float s, float d, Color c, float w=100, float h=20, float sm=1.0f)
        : type(t), position(pos), speed(s), duration(d), color(c), width(w), height(h), speedMultiplier(sm) {}

    void Update(float deltaTime);
    void Draw();
    bool CheckCollision(const Rectangle& rect);
};

#endif