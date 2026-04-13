#include "Particle.h"
#include "raylib.h"

Particle::Particle(Vector2 pos, Vector2 dir, float size, Color col, float life)
    : position(pos), direction(dir), size(size), color(col), lifetime(life) {}

void Particle::Update(float dt) {
    position.x += direction.x * 100.0f * dt;
    position.y += direction.y * 100.0f * dt;
    lifetime -= dt;
    size -= dt * 2.0f;
    if (size < 0) size = 0;
}

void Particle::Draw() {
    DrawCircleV(position, size, color);
}

bool Particle::IsDead() {
    return lifetime <= 0;
}

ParticleSystem::ParticleSystem() {}

void ParticleSystem::Emit(Vector2 pos, Color col, int count) {
    for (int i = 0; i < count; i++) {
        float dx = (rand() % 200 - 100) / 100.0f;
        float dy = (rand() % 200 - 100) / 100.0f;
        float sz = (rand() % 50 + 20) / 10.0f;
        particles.emplace_back(pos, Vector2{dx, dy}, sz, col, 0.5f);
    }
}

void ParticleSystem::Update() {
    float dt = GetFrameTime();
    for (auto it = particles.begin(); it != particles.end();) {
        it->Update(dt);
        if (it->IsDead()) it = particles.erase(it);
        else ++it;
    }
}

void ParticleSystem::Draw() {
    for (auto& p : particles) p.Draw();
}