#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "Particle.h"
#include "PowerUp.h"  // 必须包含
#include <vector>

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_READY,
    GAME_OVER
};

enum class Difficulty {
    EASY,
    HARD,
    HELL
};

class Game {
private:
    int screenWidth;
    int screenHeight;
    int uiHeight;
    float paddleMoveSpeed;
    float initialSpeed;
    float baseSpeed;
    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    ParticleSystem particles;
    std::vector<PowerUp> powerUps;  // 必须有
    int score;
    int lives;
    GameState currentState;
    Difficulty currentDifficulty;
    int currentLevel;

    // 这些计时变量 Game.cpp 里用到了，必须声明
    float paddleTimer;
    float slowTimer;
    float originalPaddleW;
    float originalBallSpeed;

    void GenerateBricks();
    void ResetGame();
    void ResetBall();
    void NextLevel();
    void DrawDifficultyMenu();
    void SpawnPowerUp(Vector2 pos);
    void UpdatePowerUps(float dt);
    void DrawPowerUps();
    void ApplyPowerUp(PowerUp& pu);
    void UpdatePowerUpTimers(float dt);

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