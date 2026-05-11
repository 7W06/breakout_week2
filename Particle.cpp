#include "Particle.h"

Particle::Particle()
    : position({0,0}), direction({0,0}), speed(0), color(WHITE), life(0) {}

Particle::Particle(Vector2 pos, Vector2 dir, float sp, Color col, float lf)
    : position(pos), direction(dir), speed(sp), color(col), life(lf) {}

void Particle::Update(float dt) {
    position.x += direction.x * speed * dt;
    position.y += direction.y * speed * dt;
    life -= dt;
}

void Particle::Draw() {
    DrawPixelV(position, color);
}

bool Particle::IsDead() const {
    return life <= 0;
}