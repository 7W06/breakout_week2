#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"

enum class PowerUpType {
    LENGTHEN,
    MULTI_BALL,
    SLOW_BALL
};

struct PowerUp {
    PowerUpType type;
    Rectangle rect;
    float speed;
    float timer;

    PowerUp(Vector2 pos, PowerUpType t);
    void Update(float dt);
    void Draw();
    bool CheckCollision(Rectangle paddleRect);
};

#endif