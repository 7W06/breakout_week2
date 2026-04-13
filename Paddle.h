#ifndef PADDLE_H
#define PADDLE_H
#include "raylib.h"

class Paddle {
private:
    Rectangle rect;
    int screenWidth;
public:
    Paddle(float x, float y, float w, float h, int screenW);
    void Draw();
    void MoveLeft(float speed);
    void MoveRight(float speed);
    Rectangle GetRect() const;
};

#endif