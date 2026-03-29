#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "Particle.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

int main() {
    std::srand(std::time(0)); 
    const int screenWidth = 1600;
    const int screenHeight = 800;
    const int uiHeight = 60;
    InitWindow(screenWidth, screenHeight, "Brick Breaker - Ultimate Edition");
    SetTargetFPS(60);
    HideCursor(); // 隐藏鼠标（可选，可删除）

    Ball ball({screenWidth/2, screenHeight/2}, {3, 3}, 10);
    Paddle paddle(screenWidth/2 - 50, screenHeight - uiHeight - 50, 100, 20);
    
    std::vector<Brick> bricks;
    ParticleSystem particles;

    auto generateBricks = [&]() {
        bricks.clear();
        const float brickWidth = 80;
        const float brickHeight = 30;
        const float brickGap = 10;
        const int brickRows = 5;
        const int brickCols = 8;
        float startX = (screenWidth - (brickCols * brickWidth + (brickCols-1)*brickGap)) / 2;
        float startY = 50;
        for (int row = 0; row < brickRows; row++) {
            for (int col = 0; col < brickCols; col++) {
                float x = startX + col * (brickWidth + brickGap);
                float y = startY + row * (brickHeight + brickGap);
                int type = 0;
                
                int randVal = std::rand() % 100; 
                if (randVal < 5) type = 3;       // 5% 概率金砖
                else if (randVal < 15) type = 2; // 10% 概率困难
                else if (randVal < 45) type = 1; // 30% 概率中等
                else type = 0;
                bricks.emplace_back(x, y, brickWidth, brickHeight, type);
            }
        }
    };
    generateBricks();

    int score = 0;
    int lives = 3;
    bool gameOver = false;
    bool isPaused = false;
    bool isStarted = false;
    float baseSpeed = 3.0f;
    const float paddleMoveSpeed = 10.0f; // 新增：挡板键盘移动速度

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;

        // 游戏重启逻辑
        if (gameOver && IsKeyPressed(KEY_R)) {
            lives = 3; score = 0; gameOver = false; isPaused = false; isStarted = false; baseSpeed = 3.0f;
            ball = Ball({screenWidth/2, screenHeight/2}, {baseSpeed, baseSpeed}, 10);
            generateBricks();
        }

        // 暂停继续逻辑
        if (isPaused && IsKeyPressed(KEY_SPACE)) {
            isPaused = false;
            isStarted = true;
            ball = Ball({screenWidth/2, screenHeight/2}, {baseSpeed, baseSpeed}, 10);
        }

        // 速度调节逻辑
        if (IsKeyPressed(KEY_EQUAL) && !gameOver) { baseSpeed += 0.5f; ball.SetSpeed({baseSpeed, baseSpeed}); }
        if (IsKeyPressed(KEY_MINUS) && !gameOver && baseSpeed > 1.0f) { baseSpeed -= 0.5f; ball.SetSpeed({baseSpeed, baseSpeed}); }

        if (!gameOver) {
            // ========== 核心修改：删除鼠标控制，改为键盘方向键控制 ==========
            if (IsKeyDown(KEY_LEFT)) { // 左方向键：挡板左移
                paddle.MoveLeft(paddleMoveSpeed);
            }
            if (IsKeyDown(KEY_RIGHT)) { // 右方向键：挡板右移
                paddle.MoveRight(paddleMoveSpeed);
            }
            // ==============================================================

            // 小球未发射时，跟随挡板中心
            if (!isStarted) {
                ball.SetPosition({ paddle.GetRect().x + paddle.GetRect().width/2, paddle.GetRect().y - 10 });
                // 新增：按空格发射小球（原逻辑保留，适配键盘操作）
                if (IsKeyPressed(KEY_SPACE)) {
                    isStarted = true;
                }
            } else {
                ball.Move();
                ball.BounceEdge(screenWidth, screenHeight - uiHeight);
                ball.BouncePaddle(paddle);

                // 小球与砖块碰撞逻辑
                for (auto& brick : bricks) {
                    if (ball.BounceBrick(brick)) {
                        if (!brick.IsActive()) {
                            Vector2 brickCenter = { brick.GetRect().x + brick.GetRect().width/2, 
                                                    brick.GetRect().y + brick.GetRect().height/2 };
                            particles.Emit(brickCenter, brick.GetColor(), 15);
                            
                            score += brick.CalculateScore(); 
                            // 每100分提升小球速度
                            if (score % 100 == 0) {
                                baseSpeed *= 1.05f;
                                ball.SetSpeed({baseSpeed, baseSpeed});
                            }
                        }
                        break;
                    }
                }

                // 小球掉落，减少生命
                if (ball.GetSpeed().y > 0 && ball.GetPosition().y + ball.GetRadius() >= screenHeight - uiHeight) {
                    lives--;
                    isPaused = true;
                    isStarted = false;
                    ball.SetSpeed({baseSpeed, baseSpeed});
                    if (lives <= 0) { gameOver = true; isPaused = false; }
                }
            }

            // 判断是否所有砖块被击碎（胜利条件）
            bool allBricksBroken = true;
            for (const auto& brick : bricks) {
                if (brick.IsActive()) { allBricksBroken = false; break; }
            }
            if (allBricksBroken) {
                gameOver = true; 
                isStarted = false;
            }
        }

        // 粒子特效更新
        particles.Update();

        // 绘制逻辑
        BeginDrawing();
        ClearBackground({ 245, 245, 250, 255 });

        // 绘制UI区域和边界
        DrawRectangle(0, screenHeight - uiHeight, screenWidth, uiHeight, { 230, 230, 235, 255 });
        DrawLine(0, screenHeight - uiHeight, screenWidth, screenHeight - uiHeight, GRAY);
        DrawRectangle(0, 0, 5, screenHeight, { 200, 200, 205, 255 });
        DrawRectangle(screenWidth-5, 0, 5, screenHeight, { 200, 200, 205, 255 });
        DrawRectangle(0, 0, screenWidth, 5, { 200, 200, 205, 255 });

        // 绘制游戏元素
        for (auto& brick : bricks) brick.Draw();
        paddle.Draw();
        ball.Draw();
        particles.Draw();

        // ========== 修改：删除鼠标圆点绘制（可选，可保留） ==========
        // DrawCircle(GetMouseX(), GetMouseY(), 5, DARKGRAY); 
        // =============================================================

        // 绘制计分、生命、速度
        DrawText(TextFormat("Score: %d", score), 20, 20, 28, DARKGRAY);
        DrawText(TextFormat("Lives: %d", lives), screenWidth - 120, 20, 28, DARKGRAY);
        DrawText(TextFormat("Speed: %.1f", baseSpeed), 20, 55, 20, GRAY);
        
        // ========== 修改：更新操作提示，移除鼠标说明，改为键盘说明 ==========
        DrawText("←→: Move | Space: Launch", 20, screenHeight - 45, 18, GRAY);
        // ==============================================================
        DrawText("Golden Brick ($): 100pts (<10s), 50pts (<20s)", 450, screenHeight - 30, 18, GOLD);

        // 未开始游戏提示
        if (!isStarted && !gameOver) {
            DrawText("Press SPACE to Launch", screenWidth/2 - 180, screenHeight - uiHeight - 100, 25, LIGHTGRAY);
        }

        // 暂停提示
        if (isPaused) {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 100 });
            DrawText("PAUSED", screenWidth/2 - 80, screenHeight/2 - 30, 40, WHITE);
            DrawText("Press SPACE to continue", screenWidth/2 - 160, screenHeight/2 + 20, 20, LIGHTGRAY);
        }

        // 游戏结束（失败/胜利）提示
        if (gameOver) {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 150 });
            const char* gameOverText = lives <= 0 ? "GAME OVER!" : "YOU WIN!";
            DrawText(gameOverText, screenWidth/2 - 120, screenHeight/2 - 60, 40, lives <= 0 ? RED : GREEN);
            DrawText(TextFormat("Final Score: %d", score), screenWidth/2 - 100, screenHeight/2, 24, WHITE);
            DrawText("Press R to RESTART", screenWidth/2 - 110, screenHeight/2 + 40, 20, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}