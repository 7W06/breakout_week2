#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"

enum class PowerUpType {
    LENGTHEN,
    MULTI_BALL,
    SLOW_BALL
};

class PowerUp {
public:
    Rectangle rect;
    PowerUpType type;
    Vector2 speed;

    PowerUp(Vector2 pos, PowerUpType t);

    void Update(float dt);
    bool CheckCollision(Rectangle r);
    void Draw();
};

#endif