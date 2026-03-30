#include "Game.h"
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game() : currentState(GameState::MENU) {
    LoadConfig();
    std::srand(time(0));
}

Game::~Game() { CloseWindow(); }

void Game::LoadConfig() {
    std::ifstream f("config.json");
    json cfg = json::parse(f);

    screenWidth = cfg["screenWidth"];
    screenHeight = cfg["screenHeight"];
    uiHeight = cfg["uiHeight"];
    paddleMoveSpeed = cfg["paddleSpeed"];

    float bx = cfg["ballBaseSpeedX"];
    float by = cfg["ballBaseSpeedY"];
    int r = cfg["ballRadius"];

    Vector2 bPos = {(float)screenWidth/2, (float)(screenHeight - uiHeight - 60)};
    ball = Ball(bPos, {bx, by}, r);

    paddle = Paddle(
        (float)screenWidth/2 - 50,
        (float)(screenHeight - uiHeight - 50),
        100, 20
    );

    lives = cfg["initialLives"];
    baseSpeed = cfg["initialSpeed"];
    score = 0;
}

void Game::Init() {
    InitWindow(screenWidth, screenHeight, "BrickBreaker");
    SetTargetFPS(60);
    SetWindowState(FLAG_VSYNC_HINT);
    HideCursor();
    GenerateBricks();
}

void Game::GenerateBricks() {
    bricks.clear();
    const float w=80, h=30, gap=10;
    const int rows=5, cols=8;
    float x0 = (screenWidth - (cols*w + (cols-1)*gap))*0.5f;
    float y0 = 50;

    for(int r=0;r<rows;r++){
        for(int c=0;c<cols;c++){
            float x = x0 + c*(w+gap);
            float y = y0 + r*(h+gap);
            int rv = rand()%100;
            int type=0;
            if(rv<5) type=3;
            else if(rv<15) type=2;
            else if(rv<45) type=1;
            else type=0;
            bricks.emplace_back(x,y,w,h,type);
        }
    }
}

void Game::ResetGame() {
    score=0;
    LoadConfig();
    GenerateBricks();
    currentState = GameState::PLAYING;
}

void Game::HandleInput() {
    if(IsKeyPressed(KEY_ESCAPE)) { CloseWindow(); return; }

    switch(currentState) {
        case GameState::MENU:
            if(IsKeyPressed(KEY_SPACE)) currentState=GameState::PLAYING;
            break;
        case GameState::PLAYING:
            if(IsKeyDown(KEY_LEFT)) paddle.MoveLeft(paddleMoveSpeed);
            if(IsKeyDown(KEY_RIGHT)) paddle.MoveRight(paddleMoveSpeed);
            if(IsKeyPressed(KEY_P)) currentState=GameState::PAUSED;
            if(IsKeyPressed(KEY_EQUAL)) { baseSpeed+=0.5f; ball.SetSpeed({baseSpeed,baseSpeed}); }
            if(IsKeyPressed(KEY_MINUS)&&baseSpeed>1) { baseSpeed-=0.5f; ball.SetSpeed({baseSpeed,baseSpeed}); }
            break;
        case GameState::PAUSED:
            if(IsKeyPressed(KEY_SPACE)) currentState=GameState::PLAYING;
            break;
        case GameState::GAME_OVER:
            if(IsKeyPressed(KEY_R)) ResetGame();
            break;
    }
}

void Game::Update() {
    if(currentState != GameState::PLAYING) return;

    ball.Move();
    ball.BounceEdge(screenWidth, screenHeight - uiHeight);
    ball.BouncePaddle(paddle);
    particles.Update();

    for(auto& brick : bricks) {
        if(ball.BounceBrick(brick)) {
            if(!brick.IsActive()) {
                Vector2 cen = {brick.GetRect().x+brick.GetRect().width/2, brick.GetRect().y+brick.GetRect().height/2};
                particles.Emit(cen, brick.GetColor(),5);
                score += brick.CalculateScore();
                if(score%100==0) { baseSpeed*=1.05f; ball.SetSpeed({baseSpeed,baseSpeed}); }
            }
            break;
        }
    }

    if(ball.GetPosition().y + ball.GetRadius() >= screenHeight - uiHeight) {
        lives--;
        ball = Ball({(float)screenWidth/2, (float)(screenHeight-uiHeight-60)},{baseSpeed,baseSpeed},10);
        if(lives<=0) currentState=GameState::GAME_OVER;
    }

    bool all=true;
    for(auto& b:bricks) if(b.IsActive()) {all=false;break;}
    if(all) currentState=GameState::GAME_OVER;
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground({245,245,250,255});

    DrawRectangle(0,screenHeight-uiHeight,screenWidth,uiHeight,{230,230,235,255});
    DrawLine(0,screenHeight-uiHeight,screenWidth,screenHeight-uiHeight,GRAY);

    DrawText(TextFormat("Score:%d",score),20,20,28,DARKGRAY);
    DrawText(TextFormat("Lives:%d",lives),screenWidth-120,20,28,DARKGRAY);
    DrawText(TextFormat("Speed:%.1f",baseSpeed),20,55,20,GRAY);

    switch(currentState) {
        case GameState::MENU:
            DrawText("BRICK BREAKER",screenWidth/2-220,screenHeight/2-100,60,DARKBLUE);
            DrawText("SPACE to START",screenWidth/2-160,screenHeight/2+40,30,LIGHTGRAY);
            break;
        case GameState::PLAYING:
            for(auto&b:bricks)b.Draw();paddle.Draw();ball.Draw();particles.Draw();
            break;
        case GameState::PAUSED:
            for(auto&b:bricks)b.Draw();paddle.Draw();ball.Draw();part.Draw();
            DrawRectangle(0,0,screenWidth,screenHeight,{0,0,0,120});
            DrawText("PAUSED",screenWidth/2-80,screenHeight/2-30,40,WHITE);
            break;
        case GameState::GAME_OVER:
            DrawRectangle(0,0,screenWidth,screenHeight,{0,0,0,160});
            const char* t = lives<=0?"GAME OVER":"YOU WIN!";
            Color c = lives<=0?RED:GREEN;
            DrawText(t,screenWidth/2-140,screenHeight/2-60,50,c);
            DrawText("R to Restart",screenWidth/2-120,screenHeight/2+50,24,LIGHTGRAY);
            break;
    }
    EndDrawing();
}

bool Game::ShouldQuit() { return WindowShouldClose(); }