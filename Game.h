#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "Particle.h"
#include "PowerUp.h"
#include <vector>
#include <mutex>
#include <atomic>

enum class GameState { MENU, PLAYING, PAUSED, GAME_READY, GAME_OVER };
enum class Difficulty { EASY, HARD, HELL };

class Game {
private:
    int screenWidth;
    int screenHeight;
    int uiHeight;

    float paddleMoveSpeed;
    float initialSpeed;
    float baseSpeed;
    float originalBallSpeed;

    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<Brick> bricks;
    std::vector<Particle> particles;
    std::vector<PowerUp> powerUps;

    int score;
    int lives;
    GameState currentState;
    Difficulty currentDifficulty;
    int currentLevel;

    float paddleTimer;
    float slowTimer;
    float originalPaddleW;

    std::atomic<bool> isLoading;
    std::atomic<bool> loadComplete;
    std::mutex loadMutex;

    void GenerateBricks();
    void ResetBall();
    void NextLevel();
    void SpawnPowerUp(Vector2 pos);
    void ApplyPowerUp(PowerUp& pu);
    void UpdatePowerUps(float dt);
    void UpdatePowerUpTimers(float dt);

public:
    Game();
    ~Game() = default;

    void Init();
    void HandleInput();
    void Update();
    void Draw();
    bool ShouldQuit();
    void LoadTextureAsync();
    void ResetGame();
};

#endif