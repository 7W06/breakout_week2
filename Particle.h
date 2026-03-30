#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include <vector>
#include <algorithm>

struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    int life;

    Particle(Vector2 position, Color col) {
        pos = position;
        color = col;
        vel = {(float)GetRandomValue(-2,2), (float)GetRandomValue(-3,-1)};
        life = 12;
    }

    void Update() {
        pos.x += vel.x;
        pos.y += vel.y;
        life--;
    }

    void Draw() {
        DrawPixelV(pos, color);
    }

    bool IsDead() { return life <= 0; }
};

class ParticleSystem {
public:
    std::vector<Particle> particles;

    void Emit(Vector2 pos, Color color, int count=5) {
        for(int i=0;i<count;i++)
            particles.emplace_back(pos, color);
    }

    void Update() {
        for(auto& p : particles) p.Update();
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](Particle& p){ return p.IsDead(); }),
            particles.end()
        );
    }

    void Draw() {
        for(auto& p : particles) p.Draw();
    }
};

#endif