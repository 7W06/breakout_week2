#ifndef BRICK_H
#define BRICK_H
#include "raylib.h"

class Brick {
private:
    Rectangle rect;
    bool active;
    int type;
    int health;
    int maxHealth;
    Color color;
    double spawnTime;
public:
    Brick(float x, float y, float w, float h, int t);
    void Draw();
    void Hit();
    bool IsActive() const;
    Rectangle GetRect() const;
    Color GetColor() const;
    int CalculateScore();
};

#endif