#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game()
    : screenWidth(1600)
    , screenHeight(800)
    , uiHeight(60)
    , paddleMoveSpeed(10.0f)
    , initialSpeed(3.0f)
    , baseSpeed(3.0f)
    , ball({(float)screenWidth/2, (float)(screenHeight - uiHeight - 60)},{initialSpeed,-initialSpeed},10)
    , paddle((float)screenWidth/2 - 50, (float)(screenHeight - uiHeight - 50),100,20,screenWidth)
    , score(0), lives(3)
    , currentState(GameState::MENU)
    , currentDifficulty(Difficulty::EASY)
    , currentLevel(1)
    , paddleTimer(0), slowTimer(0)
{
    std::srand(time(0));
    originalPaddleW = paddle.GetRect().width;
    originalBallSpeed = baseSpeed;
}

Game::~Game() { CloseWindow(); }

void Game::Init() {
    InitWindow(screenWidth, screenHeight, "BrickBreaker 关卡+难度+道具");
    SetTargetFPS(60);
    SetWindowState(FLAG_VSYNC_HINT);
    HideCursor();
    GenerateBricks();
}

void Game::GenerateBricks() {
    bricks.clear();
    int rows = 4, cols = 7;
    if (currentDifficulty == Difficulty::HARD) { rows=5; cols=9; }
    if (currentDifficulty == Difficulty::HELL) { rows=6; cols=11; }

    float w = 80 - currentLevel*2; if(w<40)w=40;
    float h = 30;
    float gap = 10 - currentLevel; if(gap<3)gap=3;

    float x0 = (screenWidth - (cols*w + (cols-1)*gap))*0.5f;
    float y0 = 50 + currentLevel*10;

    for(int r=0;r<rows;r++){
        for(int c=0;c<cols;c++){
            float x = x0 + c*(w+gap);
            float y = y0 + r*(h+gap);
            int rv = rand()%100;
            int type=0;
            if(currentDifficulty==Difficulty::EASY){
                if(rv<3)type=3; else if(rv<10)type=2; else if(rv<40)type=1;
            }else if(currentDifficulty==Difficulty::HARD){
                if(rv<8)type=3; else if(rv<20)type=2; else if(rv<50)type=1;
            }else{
                if(rv<15)type=3; else if(rv<35)type=2; else if(rv<65)type=1;
            }
            bricks.emplace_back(x,y,w,h,type);
        }
    }
}

void Game::ResetGame(){
    score=0; currentLevel=1;
    if(currentDifficulty==Difficulty::EASY){lives=5; initialSpeed=2.5;}
    else if(currentDifficulty==Difficulty::HARD){lives=3; initialSpeed=3.5;}
    else{lives=2; initialSpeed=4.5;}
    baseSpeed=initialSpeed;
    originalBallSpeed=baseSpeed;
    paddleTimer=0; slowTimer=0;
    GenerateBricks();
    powerUps.clear();
    ball=Ball({(float)screenWidth/2,(float)(screenHeight-uiHeight-60)},{baseSpeed,-baseSpeed},10);
    currentState=GameState::MENU;
}

void Game::ResetBall(){
    ball=Ball({(float)screenWidth/2,(float)(screenHeight-uiHeight-60)},{baseSpeed,-baseSpeed},10);
}

void Game::NextLevel(){
    currentLevel++;
    baseSpeed*=1.1f;
    originalBallSpeed=baseSpeed;
    GenerateBricks();
    ResetBall();
    currentState=GameState::GAME_READY;
}

void Game::HandleInput(){
    if(IsKeyPressed(KEY_ESCAPE)){CloseWindow();return;}
    switch(currentState){
        case GameState::MENU:
            if(IsKeyPressed(KEY_ONE)){currentDifficulty=Difficulty::EASY; ResetGame(); currentState=GameState::GAME_READY;}
            if(IsKeyPressed(KEY_TWO)){currentDifficulty=Difficulty::HARD; ResetGame(); currentState=GameState::GAME_READY;}
            if(IsKeyPressed(KEY_THREE)){currentDifficulty=Difficulty::HELL; ResetGame(); currentState=GameState::GAME_READY;}
            break;
        case GameState::PLAYING:
            if(IsKeyDown(KEY_LEFT))paddle.MoveLeft(paddleMoveSpeed);
            if(IsKeyDown(KEY_RIGHT))paddle.MoveRight(paddleMoveSpeed);
            if(IsKeyPressed(KEY_P))currentState=GameState::PAUSED;
            break;
        case GameState::PAUSED:
            if(IsKeyPressed(KEY_SPACE))currentState=GameState::PLAYING;
            break;
        case GameState::GAME_READY:
            if(IsKeyPressed(KEY_SPACE)){ResetBall();currentState=GameState::PLAYING;}
            break;
        case GameState::GAME_OVER:
            if(IsKeyPressed(KEY_R))ResetGame();
            break;
    }
}

void Game::SpawnPowerUp(Vector2 pos){
    int r = rand()%100;
    PowerUpType t;
    if(r<35) t = PowerUpType::LENGTHEN;
    else if(r<65) t = PowerUpType::MULTI_BALL;
    else t = PowerUpType::SLOW_BALL;
    powerUps.emplace_back(pos,t);
}

void Game::UpdatePowerUps(float dt){
    for(auto it=powerUps.begin();it!=powerUps.end();){
        it->Update(dt);
        if(it->CheckCollision(paddle.GetRect())){
            ApplyPowerUp(*it);
            it=powerUps.erase(it);
        }else if(it->rect.y>screenHeight){
            it=powerUps.erase(it);
        }else{
            ++it;
        }
    }
}

void Game::ApplyPowerUp(PowerUp& pu){
    if(pu.type==PowerUpType::LENGTHEN){
        paddleTimer=8.0f;
        paddle.SetWidth(150);
    }else if(pu.type==PowerUpType::SLOW_BALL){
        slowTimer=5.0f;
        baseSpeed=originalBallSpeed*0.5f;
        ball.SetSpeed({baseSpeed,-baseSpeed});
    }
}

void Game::UpdatePowerUpTimers(float dt){
    if(paddleTimer>0){
        paddleTimer-=dt;
        if(paddleTimer<=0)paddle.SetWidth(originalPaddleW);
    }
    if(slowTimer>0){
        slowTimer-=dt;
        if(slowTimer<=0){
            baseSpeed=originalBallSpeed;
            ball.SetSpeed({baseSpeed,-baseSpeed});
        }
    }
}

void Game::DrawPowerUps(){
    for(auto& p:powerUps)p.Draw();
}

void Game::Update(){
    if(currentState!=GameState::PLAYING)return;
    float dt=GetFrameTime();
    UpdatePowerUps(dt);
    UpdatePowerUpTimers(dt);
    ball.Move();
    ball.BounceEdge(screenWidth,screenHeight-uiHeight);
    ball.BouncePaddle(paddle);
    particles.Update();

    for(auto& brick:bricks){
        if(brick.IsActive()&&ball.BounceBrick(brick)){
            Vector2 cen={brick.GetRect().x+brick.GetRect().width/2,brick.GetRect().y+brick.GetRect().height/2};
            particles.Emit(cen,brick.GetColor(),8);
            if(rand()%100<25) SpawnPowerUp(cen);
            if(!brick.IsActive())score+=brick.CalculateScore();
            break;
        }
    }

    if(ball.GetPosition().y+ball.GetRadius()>=screenHeight-uiHeight){
        lives--;
        if(lives>0)currentState=GameState::GAME_READY;
        else currentState=GameState::GAME_OVER;
    }

    bool all=true;
    for(auto&b:bricks)if(b.IsActive()){all=false;break;}
    if(all)NextLevel();
}

void Game::DrawDifficultyMenu(){
    DrawText("SELECT MODE",screenWidth/2-180,screenHeight/2-120,50,DARKBLUE);
    DrawText("1 - EASY",screenWidth/2-100,screenHeight/2-40,30,GREEN);
    DrawText("2 - HARD",screenWidth/2-100,screenHeight/2+10,30,ORANGE);
    DrawText("3 - HELL",screenWidth/2-100,screenHeight/2+60,30,RED);
}

void Game::Draw(){
    BeginDrawing();
    ClearBackground({245,245,250,255});
    DrawRectangle(0,screenHeight-uiHeight,screenWidth,uiHeight,{230,230,235,255});
    DrawLine(0,screenHeight-uiHeight,screenWidth,screenHeight-uiHeight,GRAY);

    DrawText(TextFormat("Score:%d",score),20,20,28,DARKGRAY);
    DrawText(TextFormat("Lives:%d",lives),screenWidth-120,20,28,DARKGRAY);
    DrawText(TextFormat("Level:%d",currentLevel),180,20,28,DARKGRAY);
    DrawText(TextFormat("Speed:%.1f",baseSpeed),20,55,20,GRAY);

    const char* diff="EASY";
    if(currentDifficulty==Difficulty::HARD)diff="HARD";
    if(currentDifficulty==Difficulty::HELL)diff="HELL";
    DrawText(diff,screenWidth/2-50,20,28,RED);

    DrawText("<- -> Move | P Pause | R Restart",20,screenHeight-40,18,DARKGRAY);

    switch(currentState){
        case GameState::MENU: DrawDifficultyMenu(); break;
        case GameState::PLAYING:
            for(auto&b:bricks)b.Draw();
            paddle.Draw(); ball.Draw();
            particles.Draw(); DrawPowerUps();
            break;
        case GameState::PAUSED:
            for(auto&b:bricks)b.Draw();paddle.Draw();ball.Draw();particles.Draw();DrawPowerUps();
            DrawRectangle(0,0,screenWidth,screenHeight,{0,0,0,120});
            DrawText("PAUSED",screenWidth/2-80,screenHeight/2-30,40,WHITE);
            break;
        case GameState::GAME_READY:
            for(auto&b:bricks)b.Draw();paddle.Draw();particles.Draw();DrawPowerUps();
            DrawCircleV({(float)screenWidth/2,(float)(screenHeight-uiHeight-60)},10,MAROON);
            DrawRectangle(0,0,screenWidth,screenHeight,{0,0,0,80});
            DrawText("PRESS SPACE",screenWidth/2-140,screenHeight/2-30,40,ORANGE);
            break;
        case GameState::GAME_OVER:
            DrawRectangle(0,0,screenWidth,screenHeight,{0,0,0,160});
            const char* t=lives<=0?"GAME OVER":"YOU WIN!";
            Color col=lives<=0?RED:GREEN;
            DrawText(t,screenWidth/2-140,screenHeight/2-60,50,col);
            DrawText(TextFormat("Score:%d",score),screenWidth/2-120,screenHeight/2,28,WHITE);
            DrawText("PRESS R TO RESTART",screenWidth/2-130,screenHeight/2+50,24,LIGHTGRAY);
            break;
    }
    EndDrawing();
}

bool Game::ShouldQuit(){return WindowShouldClose();}