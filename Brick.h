#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"

class Brick {
private:
    Rectangle rect;
    int type;
    int health;
    int maxHealth;
    bool active;
    Color color;
    double spawnTime;       // 你的原版：倒计时砖块出生时间
    double expireTime;      // 你的原版：倒计时总时长

public:
    Brick(float x, float y, float w, float h, int t);
    void Draw();
    void Hit();
    bool IsActive() const;
    Rectangle GetRect() const;
    Color GetColor() const;
    int CalculateScore();
    void SetColor(Color c);
};

#endif