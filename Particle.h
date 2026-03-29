#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include <vector>
#include <algorithm>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float maxLife;
    float size;

    Particle(Vector2 pos, Color col) {
        position = pos;
        color = col;
        velocity = { (float)(GetRandomValue(-5, 5)), (float)(GetRandomValue(-8, -2)) };
        life = 30.0f;
        maxLife = 30.0f;
        size = (float)GetRandomValue(3, 6);
    }

    void Update() {
        position.x += velocity.x;
        position.y += velocity.y;
        velocity.y += 0.3f;
        life--;
    }

    void Draw() {
        float alpha = life / maxLife;
        Color c = { color.r, color.g, color.b, (unsigned char)(alpha * 255) };
        DrawRectangleRec({ position.x, position.y, size, size }, c);
    }

    bool IsDead() const { return life <= 0; }
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
public:
    void Emit(Vector2 pos, Color color, int count) {
        for (int i = 0; i < count; i++) {
            particles.emplace_back(pos, color);
        }
    }

    void Update() {
        for (auto& p : particles) p.Update();
        particles.erase(std::remove_if(particles.begin(), particles.end(), 
            [](const Particle& p) { return p.IsDead(); }), particles.end());
    }

    void Draw() {
        for (auto& p : particles) p.Draw();
    }
};

#endif
