#ifndef BALL_H
#define BALL_H

#include "raylib.h"

class Ball {
private:
    Vector2 position;
    Vector2 speed;
    float radius;

public:
    Ball(Vector2 pos, Vector2 spd, float rad);

    void Move();
    void BounceEdge(int screenWidth, int screenHeight);
    void BouncePaddle(class Paddle& paddle);
    bool BounceBrick(class Brick& brick);

    Vector2 GetPosition() { return position; }
    float GetRadius() { return radius; }
    Vector2 GetSpeed() { return speed; }
    void SetSpeed(Vector2 spd) { speed = spd; }

    void Draw();
};

#endif