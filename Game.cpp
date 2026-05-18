#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>

Game::Game()
    : screenWidth(1600), screenHeight(800), uiHeight(60),
      paddleMoveSpeed(10.0f), initialSpeed(3.0f), baseSpeed(3.0f),
      paddle((float)screenWidth / 2 - 50, (float)(screenHeight - uiHeight - 50), 100, 20, screenWidth),
      score(0), lives(3), currentState(GameState::MENU),
      currentDifficulty(Difficulty::EASY), currentLevel(1),
      paddleTimer(0), slowTimer(0),
      isLoading(false), loadComplete(false)
{
    originalPaddleW = paddle.GetRect().width;
    originalBallSpeed = baseSpeed;
    std::srand((unsigned int)time(NULL));
}

// 新增：读取文件内容
std::string ReadFileToString(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "警告：无法打开文件 " << path << "，使用默认配置！" << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 新增：保存字符串到文件
void WriteStringToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "警告：无法保存文件 " << path << "！" << std::endl;
        return;
    }
    file << content;
    file.close();
}

// 新增：加载关卡配置（核心JSON解析）
void Game::LoadLevelConfigs() {
    levelConfigs.clear();
    std::string jsonContent = ReadFileToString(levelsJsonPath);
    
    // JSON文件缺失/为空 → 生成默认关卡
    if (jsonContent.empty()) {
        CreateDefaultLevels();
        DrawText("警告：levels.json缺失，使用默认关卡！", 10, 50, 20, RED);
        return;
    }

    cJSON* root = cJSON_Parse(jsonContent.c_str());
    if (!root) {
        std::cerr << "警告：levels.json格式错误，使用默认配置！" << std::endl;
        CreateDefaultLevels();
        cJSON_Delete(root);
        return;
    }

    cJSON* levelsArray = cJSON_GetObjectItem(root, "levels");
    if (!cJSON_IsArray(levelsArray)) {
        std::cerr << "警告：levels.json格式错误（无levels数组），使用默认配置！" << std::endl;
        CreateDefaultLevels();
        cJSON_Delete(root);
        return;
    }

    // 解析每个关卡
    int levelCount = cJSON_GetArraySize(levelsArray);
    for (int i = 0; i < levelCount; i++) {
        cJSON* levelObj = cJSON_GetArrayItem(levelsArray, i);
        LevelConfig config;
        
        config.id = cJSON_GetObjectItem(levelObj, "id")->valueint;
        config.rows = cJSON_GetObjectItem(levelObj, "rows")->valueint;
        config.cols = cJSON_GetObjectItem(levelObj, "cols")->valueint;
        config.brick_width = (float)cJSON_GetObjectItem(levelObj, "brick_width")->valuedouble;
        config.brick_height = (float)cJSON_GetObjectItem(levelObj, "brick_height")->valuedouble;
        config.gap = (float)cJSON_GetObjectItem(levelObj, "gap")->valuedouble;
        config.y_offset = (float)cJSON_GetObjectItem(levelObj, "y_offset")->valuedouble;

        // 解析砖块布局
        cJSON* bricksArray = cJSON_GetObjectItem(levelObj, "bricks");
        if (cJSON_IsArray(bricksArray)) {
            int rowCount = cJSON_GetArraySize(bricksArray);
            for (int r = 0; r < rowCount; r++) {
                cJSON* rowObj = cJSON_GetArrayItem(bricksArray, r);
                std::vector<int> row;
                int colCount = cJSON_GetArraySize(rowObj);
                for (int c = 0; c < colCount; c++) {
                    row.push_back(cJSON_GetArrayItem(rowObj, c)->valueint);
                }
                config.bricks.push_back(row);
            }
        }

        levelConfigs.push_back(config);
    }

    cJSON_Delete(root);

    // 确保至少有3个关卡
    if (levelConfigs.size() < 3) {
        std::cerr << "警告：JSON中关卡数不足3个，补充默认关卡！" << std::endl;
        CreateDefaultLevels();
    }
}

// 新增：生成默认关卡（JSON错误时备用）
void Game::CreateDefaultLevels() {
    // 关卡1
    LevelConfig l1;
    l1.id = 1; l1.rows = 4; l1.cols = 7;
    l1.brick_width = 80; l1.brick_height = 30; l1.gap = 10; l1.y_offset = 50;
    l1.bricks = {{0,1,0,1,0,1,0}, {1,2,1,2,1,2,1}, {2,0,2,0,2,0,2}, {0,3,0,4,0,3,0}};
    levelConfigs.push_back(l1);

    // 关卡2
    LevelConfig l2;
    l2.id = 2; l2.rows = 5; l2.cols = 9;
    l2.brick_width = 70; l2.brick_height = 30; l2.gap = 8; l2.y_offset = 60;
    l2.bricks = {{1,0,1,0,1,0,1,0,1}, {0,2,0,2,0,2,0,2,0}, {3,0,3,0,4,0,3,0,3}, {2,1,2,1,2,1,2,1,2}, {1,0,1,0,1,0,1,0,1}};
    levelConfigs.push_back(l2);

    // 关卡3
    LevelConfig l3;
    l3.id = 3; l3.rows = 6; l3.cols = 11;
    l3.brick_width = 60; l3.brick_height = 30; l3.gap = 6; l3.y_offset = 70;
    l3.bricks = {{0,1,0,1,0,1,0,1,0,1,0}, {1,2,1,2,1,2,1,2,1,2,1}, {2,3,2,3,2,4,2,3,2,3,2}, {3,0,3,0,3,0,3,0,3,0,3}, {0,2,0,2,0,2,0,2,0,2,0}, {1,0,1,0,1,0,1,0,1,0,1}};
    levelConfigs.push_back(l3);
}

// 新增：从配置生成砖块
void Game::GenerateBricksFromConfig(const LevelConfig& config) {
    bricks.clear();
    float x0 = (screenWidth - (config.cols * config.brick_width + (config.cols - 1) * config.gap)) * 0.5f;

    for (int r = 0; r < config.rows; r++) {
        for (int c = 0; c < config.cols; c++) {
            float x = x0 + c * (config.brick_width + config.gap);
            float y = config.y_offset + r * (config.brick_height + config.gap);
            int type = config.bricks[r][c];
            if (type >= 0) { // 0=普通砖，1=黄砖，2=红砖，3=掉球砖，4=倒计时砖
                bricks.emplace_back(x, y, config.brick_width, config.brick_height, type);
            }
        }
    }
}

// 重写：原来的GenerateBricks改为从配置加载
void Game::GenerateBricks() {
    // 找到当前关卡的配置
    for (const auto& config : levelConfigs) {
        if (config.id == currentLevel) {
            GenerateBricksFromConfig(config);
            return;
        }
    }
    // 找不到则用默认
    CreateDefaultLevels();
    GenerateBricksFromConfig(levelConfigs[currentLevel-1]);
}

// 新增：保存存档（分数、生命、关卡）
void Game::SaveGame() {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "score", score);
    cJSON_AddNumberToObject(root, "lives", lives);
    cJSON_AddNumberToObject(root, "current_level", currentLevel);
    cJSON_AddBoolToObject(root, "is_valid", true);

    char* jsonStr = cJSON_Print(root);
    WriteStringToFile(saveJsonPath, jsonStr);
    
    cJSON_free(jsonStr);
    cJSON_Delete(root);
}

// 新增：创建默认存档
void Game::CreateDefaultSave() {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "score", 0);
    cJSON_AddNumberToObject(root, "lives", 3);
    cJSON_AddNumberToObject(root, "current_level", 1);
    cJSON_AddBoolToObject(root, "is_valid", false);

    char* jsonStr = cJSON_Print(root);
    WriteStringToFile(saveJsonPath, jsonStr);
    
    cJSON_free(jsonStr);
    cJSON_Delete(root);
}

// 新增：加载存档
bool Game::LoadGame() {
    std::string jsonContent = ReadFileToString(saveJsonPath);
    if (jsonContent.empty()) {
        CreateDefaultSave();
        return false;
    }

    cJSON* root = cJSON_Parse(jsonContent.c_str());
    if (!root) {
        std::cerr << "警告：save.json格式错误，使用新游戏！" << std::endl;
        CreateDefaultSave();
        cJSON_Delete(root);
        return false;
    }

    // 检查存档是否有效
    cJSON* isValid = cJSON_GetObjectItem(root, "is_valid");
    if (!isValid || !cJSON_IsBool(isValid) || !isValid->valueint) {
        cJSON_Delete(root);
        return false;
    }

    // 加载存档数据
    score = cJSON_GetObjectItem(root, "score")->valueint;
    lives = cJSON_GetObjectItem(root, "lives")->valueint;
    currentLevel = cJSON_GetObjectItem(root, "current_level")->valueint;

    // 加载对应关卡
    GenerateBricks();
    ResetBall();

    cJSON_Delete(root);
    return true;
}

// 新增：启动时检测存档
void Game::CheckSaveFile() {
    std::ifstream saveFile(saveJsonPath);
    if (!saveFile.is_open()) {
        CreateDefaultSave();
        currentState = GameState::MENU;
        return;
    }
    saveFile.close();

    // 检测到存档，显示提示
    currentState = GameState::LOAD_SAVE_PROMPT;
}

void Game::Init() {
    InitWindow(screenWidth, screenHeight, "Brick Breaker");
    SetTargetFPS(60);
    
    // 加载关卡配置（带错误处理）
    LoadLevelConfigs();
    // 检测存档
    CheckSaveFile();

    // 初始生成第一关（如果没有存档）
    if (currentState == GameState::MENU) {
        GenerateBricks();
        balls.emplace_back(Vector2{(float)screenWidth / 2, (float)(screenHeight - uiHeight - 60)},
                           Vector2{baseSpeed, -baseSpeed}, 10);
    }
}

void Game::ResetGame() {
    score = 0; currentLevel = 1;
    lives = (currentDifficulty == Difficulty::EASY) ? 5 : (currentDifficulty == Difficulty::HARD) ? 3 : 2;
    baseSpeed = (currentDifficulty == Difficulty::EASY) ? 2.5f : (currentDifficulty == Difficulty::HARD) ? 3.5f : 4.5f;
    originalBallSpeed = baseSpeed;
    paddleTimer = 0; slowTimer = 0;
    GenerateBricks();
    powerUps.clear();
    balls.clear();
    balls.emplace_back(Vector2{(float)screenWidth / 2, (float)(screenHeight - uiHeight - 60)},
                       Vector2{baseSpeed, -baseSpeed}, 10);
    currentState = GameState::MENU;
    
    // 重置存档为无效
    CreateDefaultSave();
}

void Game::ResetBall() {
    balls.clear();
    balls.emplace_back(Vector2{(float)screenWidth / 2, (float)(screenHeight - uiHeight - 60)},
                       Vector2{baseSpeed, -baseSpeed}, 10);
}

void Game::NextLevel() {
    currentLevel++;
    // 最多3关（可扩展）
    if (currentLevel > 3) currentLevel = 1;
    
    baseSpeed *= 1.1f;
    originalBallSpeed = baseSpeed;
    GenerateBricks();
    ResetBall();
    currentState = GameState::GAME_READY;
    
    // 通关自动保存
    SaveGame();
}

void Game::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        // 退出前自动保存
        SaveGame();
        CloseWindow();
    }

    switch (currentState) {
        case GameState::LOAD_SAVE_PROMPT:
            // 按Y加载存档，按N新游戏
            if (IsKeyPressed(KEY_Y)) {
                LoadGame();
                currentState = GameState::GAME_READY;
            }
            if (IsKeyPressed(KEY_N)) {
                ResetGame();
                currentState = GameState::MENU;
            }
            break;
        case GameState::MENU:
            if (IsKeyPressed(KEY_ONE))  { currentDifficulty = Difficulty::EASY; ResetGame(); currentState = GameState::GAME_READY; }
            if (IsKeyPressed(KEY_TWO))  { currentDifficulty = Difficulty::HARD; ResetGame(); currentState = GameState::GAME_READY; }
            if (IsKeyPressed(KEY_THREE)){ currentDifficulty = Difficulty::HELL; ResetGame(); currentState = GameState::GAME_READY; }
            break;
        case GameState::PLAYING:
            if (IsKeyDown(KEY_LEFT))  paddle.MoveLeft(paddleMoveSpeed);
            if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(paddleMoveSpeed);
            if (IsKeyPressed(KEY_P)) currentState = GameState::PAUSED;
            // 按S手动保存
            if (IsKeyPressed(KEY_S)) SaveGame();
            break;
        case GameState::PAUSED:
            if (IsKeyPressed(KEY_SPACE)) currentState = GameState::PLAYING;
            break;
        case GameState::GAME_READY:
            if (IsKeyPressed(KEY_SPACE)) { ResetBall(); currentState = GameState::PLAYING; }
            break;
        case GameState::GAME_OVER:
            if (IsKeyPressed(KEY_R)) ResetGame();
            break;
    }

    if (IsKeyPressed(KEY_L) && !isLoading) {
        isLoading = true;
        loadComplete = false;
        std::thread(&Game::LoadTextureAsync, this).detach();
    }
}

void Game::LoadTextureAsync() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::lock_guard<std::mutex> lock(loadMutex);
    loadComplete = true;
    isLoading = false;
}

void Game::SpawnPowerUp(Vector2 pos) {
    if (rand() % 4 == 0) {
        PowerUpType t = (PowerUpType)(rand() % 3);
        powerUps.emplace_back(pos, t);
    }
}

void Game::ApplyPowerUp(PowerUp& pu) {
    if (pu.type == PowerUpType::LENGTHEN) {
        paddleTimer = 8.0f;
        paddle.SetWidth(150);
    } else if (pu.type == PowerUpType::MULTI_BALL) {
        Vector2 p = balls[0].GetPosition();
        balls.emplace_back(p, Vector2{-baseSpeed*0.7f, -baseSpeed}, 10);
        balls.emplace_back(p, Vector2{ baseSpeed*0.7f, -baseSpeed}, 10);
    } else if (pu.type == PowerUpType::SLOW_BALL) {
        slowTimer = 5.0f;
        baseSpeed = originalBallSpeed * 0.5f;
        for (auto& b : balls) b.SetSpeed({baseSpeed, -baseSpeed});
    }
}

void Game::UpdatePowerUps(float dt) {
    for (auto it = powerUps.begin(); it != powerUps.end();) {
        it->Update(dt);
        if (it->CheckCollision(paddle.GetRect())) {
            ApplyPowerUp(*it);
            it = powerUps.erase(it);
        } else if (it->rect.y > screenHeight) {
            it = powerUps.erase(it);
        } else ++it;
    }
}

void Game::UpdatePowerUpTimers(float dt) {
    if (paddleTimer > 0) {
        paddleTimer -= dt;
        if (paddleTimer <= 0) paddle.SetWidth(originalPaddleW);
    }
    if (slowTimer > 0) {
        slowTimer -= dt;
        if (slowTimer <= 0) {
            baseSpeed = originalBallSpeed;
            for (auto& b : balls) b.SetSpeed({baseSpeed, -baseSpeed});
        }
    }
}

void Game::Update() {
    if (currentState != GameState::PLAYING) return;
    float dt = GetFrameTime();
    UpdatePowerUps(dt);
    UpdatePowerUpTimers(dt);

    for (auto it = particles.begin(); it != particles.end();) {
        it->Update(dt);
        if (it->IsDead()) it = particles.erase(it); else ++it;
    }

    for (auto it = balls.begin(); it != balls.end();) {
        Ball& b = *it;
        b.Move();
        b.BounceEdge(screenWidth, screenHeight - uiHeight);
        b.BouncePaddle(paddle);
        bool hit = false;

        for (auto& brick : bricks) {
            if (b.BounceBrick(brick)) {
                Vector2 cen = { brick.GetRect().x + brick.GetRect().width/2, brick.GetRect().y + brick.GetRect().height/2 };
                for (int i = 0; i < 8; i++) {
                    float dx = (rand() % 200 - 100) / 100.0f;
                    float dy = (rand() % 200 - 100) / 100.0f;
                    particles.emplace_back(cen, Vector2{dx, dy}, 4.0f, brick.GetColor(), 0.5f);
                }

                if (rand() % 100 < 25) SpawnPowerUp(cen);
                if (!brick.IsActive()) score += brick.CalculateScore();
                hit = true;
                break;
            }
        }

        if (b.GetPosition().y + b.GetRadius() >= screenHeight - uiHeight) {
            it = balls.erase(it);
        } else ++it;
        if (hit) break;
    }

    if (balls.empty()) {
        lives--;
        // 生命减少时自动保存
        SaveGame();
        
        if (lives > 0) { currentState = GameState::GAME_READY; ResetBall(); }
        else currentState = GameState::GAME_OVER;
    }

    bool allDead = true;
    for (auto& b : bricks) if (b.IsActive()) { allDead = false; break; }
    if (allDead) NextLevel();
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawFPS(30, 80);

    DrawRectangle(0, screenHeight - uiHeight, screenWidth, uiHeight, LIGHTGRAY);

    DrawText(TextFormat("SCORE: %d", score), 10, 10, 24, DARKGRAY);
    DrawText(TextFormat("LIVES: %d", lives), screenWidth - 120, 10, 24, DARKGRAY);
    DrawText(TextFormat("LEVEL: %d", currentLevel), 200, 10, 24, DARKGRAY);

    const char* diff = "EASY";
    if (currentDifficulty == Difficulty::HARD) diff = "HARD";
    if (currentDifficulty == Difficulty::HELL) diff = "HELL";
    DrawText(diff, screenWidth/2 - 30, 10, 28, RED);

    {
        std::lock_guard<std::mutex> lock(loadMutex);
        if (isLoading) DrawText("LOADING...", screenWidth/2 - 80, screenHeight/2, 30, ORANGE);
        if (loadComplete) { for (auto& b : bricks) b.SetColor(GREEN); }
    }

    switch (currentState) {
        case GameState::LOAD_SAVE_PROMPT:
            // ✅ 英文，永远不会显示问号
            DrawText("Save file found!", screenWidth/2 - 120, screenHeight/2 - 80, 40, BLUE);
            DrawText("Press Y | N", screenWidth/2 - 100, screenHeight/2, 30, DARKBLUE);
            break;

        case GameState::MENU:
            // ✅ 英文
            DrawText("PRESS 1 2 3 SELECT DIFFICULTY", screenWidth/2 - 220, screenHeight/2 - 50, 40, DARKBLUE);
            break;

        case GameState::PLAYING:
            for (auto& b : bricks) b.Draw();
            paddle.Draw();
            for (auto& b : balls) b.Draw();
            for (auto& p : particles) p.Draw();
            for (auto& pu : powerUps) pu.Draw();
            break;

        case GameState::PAUSED:
            for (auto& b : bricks) b.Draw(); paddle.Draw();
            for (auto& b : balls) b.Draw();
            DrawText("PAUSED", screenWidth/2 - 80, screenHeight/2, 40, BLACK);
            break;

        case GameState::GAME_READY:
            // ✅ 英文
            DrawText("PRESS SPACE TO START", screenWidth/2 - 180, screenHeight/2, 40, ORANGE);
            break;

        case GameState::GAME_OVER:
            DrawText("GAME OVER", screenWidth/2 - 140, screenHeight/2, 50, RED);
            DrawText("PRESS R TO RESTART", screenWidth/2 - 130, screenHeight/2 + 60, 24, DARKGRAY);
            break;
    }
    EndDrawing();
}

bool Game::ShouldQuit() {
    return WindowShouldClose();
}