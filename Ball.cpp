#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <cmath>

Ball::Ball(Vector2 pos, Vector2 spd, float r)
    : position(pos), speed(spd), radius(r) {}

void Ball::Move() {
    position.x += speed.x;
    position.y += speed.y;
}

void Ball::BounceEdge(int screenW, int screenH) {
    if (position.x - radius <= 0 || position.x + radius >= screenW)
        speed.x *= -1;
    if (position.y - radius <= 0)
        speed.y *= -1;
}

void Ball::BouncePaddle(Paddle& paddle) {
    Rectangle p = paddle.GetRect();
    if (CheckCollisionCircleRec(position, radius, p)) {
        if (speed.y > 0)
            speed.y *= -1;
    }
}

bool Ball::CheckBrickCollision(Brick& brick) {
    return CheckCollisionCircleRec(position, radius, brick.GetRect());
}

bool Ball::BounceBrick(Brick& brick) {
    if (!brick.IsActive()) return false;
    if (CheckBrickCollision(brick)) {
        speed.y *= -1;
        brick.Hit();
        return true;
    }
    return false;
}

void Ball::Draw() {
    DrawCircleV(position, radius, MAROON);
}

Vector2 Ball::GetPosition() const { return position; }
float Ball::GetRadius() const { return radius; }
void Ball::SetSpeed(Vector2 spd) { speed = spd; }