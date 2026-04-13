#include "Paddle.h"

Paddle::Paddle(float x, float y, float w, float h, int screenW) {
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
    screenWidth = screenW;
}

void Paddle::Draw() {
    DrawRectangleRec(rect, BLUE);
}

void Paddle::MoveLeft(float speed) {
    if (rect.x > 5) rect.x -= speed;
}

void Paddle::MoveRight(float speed) {
    if (rect.x + rect.width < screenWidth - 5)
        rect.x += speed;
}

Rectangle Paddle::GetRect() const { return rect; }