#include "PowerUp.h"
#include "raylib.h"

PowerUp::PowerUp(Vector2 pos, PowerUpType t)
    : type(t), speed(120.0f), timer(0.0f)
{
    rect.x = pos.x - 15;
    rect.y = pos.y;
    rect.width = 30;
    rect.height = 15;
}

void PowerUp::Update(float dt) {
    rect.y += speed * dt;
}

void PowerUp::Draw() {
    Color c = RED;
    if (type == PowerUpType::LENGTHEN) c = GREEN;
    if (type == PowerUpType::MULTI_BALL) c = YELLOW;
    if (type == PowerUpType::SLOW_BALL) c = BLUE;

    DrawRectangleRec(rect, c);
    DrawRectangleLinesEx(rect, 1, WHITE);
}

bool PowerUp::CheckCollision(Rectangle paddleRect) {
    return CheckCollisionRecs(rect, paddleRect);
}