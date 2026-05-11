#ifndef PADDLE_H
#define PADDLE_H

#include "raylib.h"

class Paddle {
private:
    Rectangle rect;
    float screenWidth;

public:
    Paddle(float x, float y, float w, float h, float sw);

    void MoveLeft(float speed);
    void MoveRight(float speed);
    Rectangle GetRect() { return rect; }
    void SetWidth(float w) { rect.width = w; }

    void Draw();
};

#endif