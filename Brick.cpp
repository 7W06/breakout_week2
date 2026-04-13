#include "Brick.h"
#include <cmath>

Brick::Brick(float x, float y, float w, float h, int t) {
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
    active = true;
    type = t;
    spawnTime = GetTime();

    switch (type) {
        case 1:
            health = 2;
            maxHealth = 2;
            color = YELLOW;
            break;
        case 2:
            health = 3;
            maxHealth = 3;
            color = RED;
            break;
        case 3:
            health = 1;
            maxHealth = 1;
            color = GOLD;
            break;
        default:
            health = 1;
            maxHealth = 1;
            color = GREEN;
            break;
    }
}

void Brick::Draw() {
    if (!active) return;
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 1, DARKGRAY);

    if (type == 3) {
        double elapsed = GetTime() - spawnTime;
        double remain = 30.0 - elapsed;
        if (remain < 0) remain = 0;

        if (remain > 0) {
            DrawText(TextFormat("%.1f", remain),
                rect.x + rect.width/2 - 12, rect.y + rect.height/2 - 8,
                14, BLACK);
        }
        if (elapsed < 20.0) {
            DrawRectangleLinesEx(rect, 2, ORANGE);
        }
    }
}

void Brick::Hit() {
    if (!active) return;
    health--;
    if (health <= 0) active = false;
}

bool Brick::IsActive() const { return active; }
Rectangle Brick::GetRect() const { return rect; }
Color Brick::GetColor() const { return color; }

int Brick::CalculateScore() {
    if (type == 3) {
        double e = GetTime() - spawnTime;
        if (e < 20) return 50;
        if (e < 30) return 25;
        return 10;
    }
    return 10;
}