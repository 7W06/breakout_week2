#include "PowerUp.h"
#include "raylib.h"

void PowerUp::Update(float deltaTime) {
    position.y += speed * deltaTime;
}

void PowerUp::Draw() {
    DrawRectangleV(position, {width, height}, color);
    DrawRectangleLinesEx({position.x, position.y, width, height}, 2, WHITE);
}

bool PowerUp::CheckCollision(const Rectangle& rect) {
    return CheckCollisionRecs({position.x, position.y, width, height}, rect);
}