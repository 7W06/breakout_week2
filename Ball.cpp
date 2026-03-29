#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"

Ball::Ball(Vector2 pos, Vector2 sp, float r) {
    position = pos;
    speed = sp;
    radius = r;
}

void Ball::Move() {
    position.x += speed.x;
    position.y += speed.y;
}

void Ball::Draw() {
    DrawCircleV(position, radius, RED);
}

void Ball::BounceEdge(int screenWidth, int screenHeight) {
    if (position.x - radius <= 0 || position.x + radius >= screenWidth) {
        speed.x *= -1;
    }
    if (position.y - radius <= 0) {
        speed.y *= -1;
    }
}

void Ball::BouncePaddle(const Paddle& paddle) {
    Rectangle paddleRect = paddle.GetRect();
    if (CheckCollisionCircleRec(position, radius, paddleRect)) {
        speed.y *= -1;
        position.y = paddleRect.y - radius;
        float collisionPoint = (position.x - paddleRect.x) / paddleRect.width;
        speed.x = (collisionPoint - 0.5f) * 8;
    }
}

bool Ball::BounceBrick(Brick& brick) {
    if (!brick.IsActive()) return false;
    Rectangle brickRect = brick.GetRect();
    if (CheckCollisionCircleRec(position, radius, brickRect)) {
        speed.y *= -1;
        brick.Hit();
        return true;
    }
    return false;
}
