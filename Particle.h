#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib.h"
#include <vector>
#include <cmath> // 关键：补上这个头文件！

struct Particle {
    Vector2 pos;
    Vector2 speed;
    Color color;
    float life;
    Particle(Vector2 p, Vector2 s, Color c)
        : pos(p), speed(s), color(c), life(1.0f) {}
};

class ParticleSystem {
private:
    std::vector<Particle> parts;
public:
    void Emit(Vector2 pos, Color c, int count) {
        for (int i=0; i<count; i++) {
            float a = GetRandomValue(0, 360) * DEG2RAD;
            float sp = GetRandomValue(1,4);
            parts.emplace_back(pos,
                // Particle.h 第25行修改为：
                 Vector2{(float)cos(a)*sp, (float)sin(a)*sp},
                c);
        }
    }

    void Update() {
        for (auto& p : parts) {
            p.pos.x += p.speed.x;
            p.pos.y += p.speed.y;
            p.life -= 0.016f;
        }
        for (auto it=parts.begin(); it!=parts.end(); ) {
            if (it->life <= 0) it = parts.erase(it);
            else ++it;
        }
    }

    void Draw() {
        for (auto& p : parts) {
            DrawCircleV(p.pos, 2, Fade(p.color, p.life));
        }
    }
};

#endif