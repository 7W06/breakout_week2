#ifndef BALL_H
#define BALL_H
#include "raylib.h"

class Paddle;
class Brick;

class Ball {
private:
    Vector2 position;
    Vector2 speed;
    float radius;
public:
    Ball(Vector2 pos, Vector2 spd, float r);
    void Move();
    void BounceEdge(int screenW, int screenH);
    void BouncePaddle(Paddle& paddle);
    bool BounceBrick(Brick& brick);
    bool CheckBrickCollision(Brick& brick);
    void Draw();
    Vector2 GetPosition() const;
    float GetRadius() const;
    void SetSpeed(Vector2 spd);
};

#endif