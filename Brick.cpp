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
        case 1: health = 2; maxHealth = 2; color = YELLOW; break;
        case 2: health = 3; maxHealth = 3; color = RED; break;
        case 3: health = 1; maxHealth = 1; color = GOLD; break;
        default: health = 1; maxHealth = 1; color = GREEN; break;
    }
}

void Brick::Draw() {
    if (active) {
        DrawRectangleRec(rect, color);
        DrawRectangleLinesEx(rect, 1, DARKGRAY);
        
        if (type == 3) {
            double elapsed = GetTime() - spawnTime;
            double remaining = 20.0 - elapsed;
            if (remaining < 0) remaining = 0;
            
            if (remaining > 0) {
                const char* timeText = TextFormat("%.1f", remaining);
                DrawText(timeText, (int)(rect.x + rect.width/2 - 15), (int)(rect.y + rect.height/2 - 6), 12, BLACK);
            }
            
            if (elapsed < 10.0) {
                DrawRectangleLinesEx(rect, 2, ORANGE);
            }
        }
    }
}

void Brick::Hit() {
    if (!active) return;
    health--;
    if (health <= 0) {
        active = false;
    }
}

int Brick::CalculateScore() {
    if (type == 3) {
        double elapsed = GetTime() - spawnTime;
        if (elapsed < 10.0) return 100;
        else if (elapsed < 20.0) return 50;
        else return 10;
    }
    return 10;
}
