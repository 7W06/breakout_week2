#include "PowerUp.h"

PowerUp::PowerUp(Vector2 pos, PowerUpType t)
    : type(t), speed({0, 3})
{
    rect = { pos.x - 10, pos.y - 10, 20, 20 };
}

void PowerUp::Update(float dt) {
    rect.y += speed.y;
}

bool PowerUp::CheckCollision(Rectangle r) {
    return CheckCollisionRecs(rect, r);
}

void PowerUp::Draw() {
    Color c = WHITE;
    switch (type) {
        case PowerUpType::LENGTHEN: c = GREEN; break;
        case PowerUpType::MULTI_BALL: c = PURPLE; break;
        case PowerUpType::SLOW_BALL: c = BLUE; break;
    }
    DrawRectangleRec(rect, c);
    DrawRectangleLinesEx(rect, 1, BLACK);
}