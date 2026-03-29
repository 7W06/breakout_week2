#ifndef BALL_H
#define BALL_H
#include "raylib.h"

class Paddle; class Brick;

class Ball {
private:
    Vector2 position;
    Vector2 speed;
    float radius;
public:
    Ball(Vector2 pos, Vector2 sp, float r);
    void Move();
    void Draw();
    void BounceEdge(int screenWidth, int screenHeight);
    void BouncePaddle(const Paddle& paddle);
    bool BounceBrick(Brick& brick);
    
    Vector2 GetSpeed() const { return speed; }
    void SetSpeed(Vector2 sp) { speed = sp; }
    void SetPosition(Vector2 pos) { position = pos; }
    Vector2 GetPosition() const { return position; }
    float GetRadius() const { return radius; }
};

#endif
