#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include <vector>

struct Particle {
    Vector2 position;
    Vector2 direction;
    float size;
    Color color;
    float lifetime;

    Particle(Vector2 pos, Vector2 dir, float s, Color c, float l);
    void Update(float dt);
    void Draw();
    bool IsDead();
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
public:
    ParticleSystem();
    void Emit(Vector2 pos, Color col, int count);
    void Update();
    void Draw();
};

#endif