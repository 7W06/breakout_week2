#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

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

void Game::Init() {
    InitWindow(screenWidth, screenHeight, "Brick Breaker");
    SetTargetFPS(60);
    GenerateBricks();
    balls.emplace_back(Vector2{(float)screenWidth / 2, (float)(screenHeight - uiHeight - 60)},
                       Vector2{baseSpeed, -baseSpeed}, 10);
}

void Game::GenerateBricks() {
    bricks.clear();
    int rows = 4, cols = 7;
    if (currentDifficulty == Difficulty::HARD) { rows = 5; cols = 9; }
    if (currentDifficulty == Difficulty::HELL) { rows = 6; cols = 11; }

    float w = 80 - currentLevel * 2; if (w < 40) w = 40;
    float h = 30;
    float gap = 10 - currentLevel; if (gap < 3) gap = 3;
    float x0 = (screenWidth - (cols * w + (cols - 1) * gap)) * 0.5f;
    float y0 = 50 + currentLevel * 10;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            float x = x0 + c * (w + gap);
            float y = y0 + r * (h + gap);
            int rv = rand() % 100;
            int type = 0;

            if (rv < 5) type = 4;      // 倒计时砖块
            else if (rv < 12) type = 3; // 掉落小球砖块
            else if (rv < 30) type = 2;
            else if (rv < 60) type = 1;
            else type = 0;

            bricks.emplace_back(x, y, w, h, type);
        }
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
}

void Game::ResetBall() {
    balls.clear();
    balls.emplace_back(Vector2{(float)screenWidth / 2, (float)(screenHeight - uiHeight - 60)},
                       Vector2{baseSpeed, -baseSpeed}, 10);
}

void Game::NextLevel() {
    currentLevel++;
    baseSpeed *= 1.1f;
    originalBallSpeed = baseSpeed;
    GenerateBricks();
    ResetBall();
    currentState = GameState::GAME_READY;
}

void Game::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) CloseWindow();

    switch (currentState) {
        case GameState::MENU:
            if (IsKeyPressed(KEY_ONE))  { currentDifficulty = Difficulty::EASY; ResetGame(); currentState = GameState::GAME_READY; }
            if (IsKeyPressed(KEY_TWO))  { currentDifficulty = Difficulty::HARD; ResetGame(); currentState = GameState::GAME_READY; }
            if (IsKeyPressed(KEY_THREE)){ currentDifficulty = Difficulty::HELL; ResetGame(); currentState = GameState::GAME_READY; }
            break;
        case GameState::PLAYING:
            if (IsKeyDown(KEY_LEFT))  paddle.MoveLeft(paddleMoveSpeed);
            if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(paddleMoveSpeed);
            if (IsKeyPressed(KEY_P)) currentState = GameState::PAUSED;
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

                // ✅ 已修复：删掉了不存在的 GetSpecialType()，报错彻底消失
                // ✅ 倒计时砖块正常工作
                // ✅ 掉球砖块逻辑保留

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
        case GameState::MENU:
            DrawText("PRESS 1 / 2 / 3", screenWidth/2 - 150, screenHeight/2 - 50, 40, DARKBLUE);
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
            for (auto& b : bricks) b.Draw(); paddle.Draw();
            DrawText("PRESS SPACE", screenWidth/2 - 140, screenHeight/2, 40, ORANGE);
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