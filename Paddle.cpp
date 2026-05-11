#include "Paddle.h"

Paddle::Paddle(float x, float y, float w, float h, float sw)
    : rect({x, y, w, h}), screenWidth(sw) {}

void Paddle::MoveLeft(float speed) {
    rect.x -= speed;
    if (rect.x < 0) rect.x = 0;
}

void Paddle::MoveRight(float speed) {
    rect.x += speed;
    if (rect.x + rect.width > screenWidth)
        rect.x = screenWidth - rect.width;
}

void Paddle::Draw() {
    DrawRectangleRec(rect, BLUE);
    DrawRectangleLinesEx(rect, 2, BLACK);
}