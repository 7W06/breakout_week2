#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <cmath>

Ball::Ball(Vector2 pos, Vector2 spd, float rad)
    : position(pos), speed(spd), radius(rad) {}

void Ball::Move() {
    position.x += speed.x;
    position.y += speed.y;
}

void Ball::BounceEdge(int screenWidth, int screenHeight) {
    if (position.x - radius < 0 || position.x + radius > screenWidth)
        speed.x *= -1;
    if (position.y - radius < 0)
        speed.y *= -1;
}

void Ball::BouncePaddle(Paddle& paddle) {
    Rectangle padRect = paddle.GetRect();
    if (CheckCollisionCircleRec(position, radius, padRect)) {
        speed.y *= -1;
        float delta = position.x - (padRect.x + padRect.width / 2);
        speed.x = delta * 0.15f;
    }
}

bool Ball::BounceBrick(Brick& brick) {
    if (!brick.IsActive()) return false;
    Rectangle br = brick.GetRect();
    if (CheckCollisionCircleRec(position, radius, br)) {
        speed.y *= -1;
        brick.Hit();
        return true;
    }
    return false;
}

void Ball::Draw() {
    DrawCircleV(position, radius, RED);
    DrawCircleLines(position.x, position.y, radius, BLACK);
}