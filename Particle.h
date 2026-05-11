#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"

class Particle {
private:
    Vector2 position;
    Vector2 direction;
    float speed;
    Color color;
    float life;

public:
    Particle();
    Particle(Vector2 pos, Vector2 dir, float sp, Color col, float lf);

    void Update(float dt);
    void Draw();
    bool IsDead() const;
};

#endif