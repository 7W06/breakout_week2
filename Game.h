#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "Particle.h"
#include "PowerUp.h"
#include "/home/ym/breakout_week2/cJSON.h"
#include <vector>
#include <mutex>
#include <atomic>
#include <string>

enum class GameState { MENU, PLAYING, PAUSED, GAME_READY, GAME_OVER, LOAD_SAVE_PROMPT };
enum class Difficulty { EASY, HARD, HELL };

// 关卡配置结构体
struct LevelConfig {
    int id;
    int rows;
    int cols;
    float brick_width;
    float brick_height;
    float gap;
    float y_offset;
    std::vector<std::vector<int>> bricks;
};

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

    // 新增：关卡配置和存档相关
    std::vector<LevelConfig> levelConfigs;
    std::string levelsJsonPath = "levels.json";
    std::string saveJsonPath = "save.json";

    // 核心新功能函数
    void LoadLevelConfigs(); // 从JSON加载关卡配置
    void GenerateBricks();
    void GenerateBricksFromConfig(const LevelConfig& config); // 从配置生成砖块
    void SaveGame(); // 保存存档
    bool LoadGame(); // 加载存档
    void CheckSaveFile(); // 启动时检测存档
    void CreateDefaultLevels(); // JSON缺失时生成默认关卡
    void CreateDefaultSave(); // 存档缺失时生成默认存档

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