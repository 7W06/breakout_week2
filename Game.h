#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "Particle.h"
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

class Game {
private:
    int screenWidth;
    int screenHeight;
    int uiHeight;
    float paddleMoveSpeed;

    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    ParticleSystem particles;

    int score;
    int lives;
    float baseSpeed;
    GameState currentState;

    void GenerateBricks();
    void ResetGame();
    void LoadConfig();

public:
    Game();
    ~Game();

    void Init();
    void HandleInput();
    void Update();
    void Draw();
    bool ShouldQuit();
};

#endif