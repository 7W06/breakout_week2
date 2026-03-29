#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"

class Brick {
private:
    Rectangle rect;
    bool active;
    int health;
    int maxHealth;
    Color color;
    int type;
    double spawnTime;

public:
    Brick(float x, float y, float w, float h, int type);
    void Draw();
    void Hit();
    bool IsActive() const { return active; }
    Rectangle GetRect() const { return rect; }
    int CalculateScore();
    Color GetColor() const { return color; }
};

#endif
