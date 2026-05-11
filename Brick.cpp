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
    expireTime = 30.0;

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
        // ========================
        // ✅ 你的原版：倒计时金砖（type=3）
        // ========================
        case 3:
            health = 1;
            maxHealth = 1;
            color = GOLD;
            expireTime = 20.0;
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

    // 普通砖块
    if (type != 3) {
        DrawRectangleRec(rect, color);
        DrawRectangleLinesEx(rect, 1, DARKGRAY);
        return;
    }

    // ========================
    // ✅ 你的原版：倒计时金砖绘制
    // ========================
    double elapsed = GetTime() - spawnTime;
    double remain = expireTime - elapsed;

    if (remain <= 0) {
        active = false;
        return;
    }

    // 金色砖块
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 2, ORANGE);

    // 显示倒计时数字
    DrawText(TextFormat("%.1f", remain),
        rect.x + rect.width/2 - 12, rect.y + rect.height/2 - 8,
        14, BLACK);
}

void Brick::Hit() {
    if (!active) return;
    health--;
    if (health <= 0) active = false;
}

bool Brick::IsActive() const { return active; }
Rectangle Brick::GetRect() const { return rect; }
Color Brick::GetColor() const { return color; }

// ========================
// ✅ 你的原版：倒计时金砖高分逻辑
// ========================
int Brick::CalculateScore() {
    if (type == 3) {
        double e = GetTime() - spawnTime;
        if (e < 20) return 50;
        if (e < 30) return 25;
        return 10;
    }
    return 10;
}

void Brick::SetColor(Color c) {
    color = c;
}