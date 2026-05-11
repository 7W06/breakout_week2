#include "raylib.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <cmath>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 120
#define PADDLE_HEIGHT 15
#define BALL_RADIUS 10

float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

struct GameData {
    float paddle1_x;
    float paddle2_x;
    float ball_x, ball_y;
    float ball_vx, ball_vy;
    int score1, score2;
};

std::atomic<bool> connected(false);
GameData data = {0};
int sock = -1;
bool is_server = false;

void server_thread() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (sockaddr*)&addr, sizeof(addr));
    listen(s, 1);
    sock = accept(s, nullptr, nullptr);
    connected = true;
    close(s);
}

void client_thread() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) connected = true;
}

void send_data() {
    if (connected) send(sock, &data, sizeof(data), 0);
}

void recv_data() {
    GameData temp;
    while (connected) {
        if (recv(sock, &temp, sizeof(temp), MSG_DONTWAIT) > 0) {
            if (is_server) {
                data.paddle2_x = temp.paddle2_x;
                data.score2 = temp.score2;
            } else {
                data = temp;
            }
        }
        usleep(10000);
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        is_server = true;
        std::thread(server_thread).detach();
    } else {
        std::thread(client_thread).detach();
    }

    data.paddle1_x = SCREEN_WIDTH/2 - PADDLE_WIDTH/2;
    data.paddle2_x = SCREEN_WIDTH/2 - PADDLE_WIDTH/2;
    data.ball_x = SCREEN_WIDTH/2;
    data.ball_y = SCREEN_HEIGHT/2;
    data.ball_vx = 4;
    data.ball_vy = -4;

    std::thread(recv_data).detach();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Multiplayer Brick Breaker");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (!connected) {
            BeginDrawing();
            ClearBackground(DARKBLUE);
            DrawText("WAITING CONNECTION...", 280, 250, 30, WHITE);
            EndDrawing();
            continue;
        }

        if (is_server) {
            if (IsKeyDown(KEY_LEFT))  data.paddle1_x -= 8;
            if (IsKeyDown(KEY_RIGHT)) data.paddle1_x += 8;
        } else {
            if (IsKeyDown(KEY_A)) data.paddle2_x -= 8;
            if (IsKeyDown(KEY_D)) data.paddle2_x += 8;
        }

        data.paddle1_x = Clamp(data.paddle1_x, 0, SCREEN_WIDTH - PADDLE_WIDTH);
        data.paddle2_x = Clamp(data.paddle2_x, 0, SCREEN_WIDTH - PADDLE_WIDTH);

        if (is_server) {
            data.ball_x += data.ball_vx;
            data.ball_y += data.ball_vy;

            if (data.ball_x <= BALL_RADIUS || data.ball_x >= SCREEN_WIDTH - BALL_RADIUS) data.ball_vx *= -1;
            if (data.ball_y <= BALL_RADIUS) data.ball_vy *= -1;

            Rectangle p1 = {data.paddle1_x, SCREEN_HEIGHT - 40, PADDLE_WIDTH, PADDLE_HEIGHT};
            if (CheckCollisionCircleRec({data.ball_x, data.ball_y}, BALL_RADIUS, p1)) {
                data.ball_vy = -fabs(data.ball_vy);
                data.score1++;
            }

            Rectangle p2 = {data.paddle2_x, 25, PADDLE_WIDTH, PADDLE_HEIGHT};
            if (CheckCollisionCircleRec({data.ball_x, data.ball_y}, BALL_RADIUS, p2)) {
                data.ball_vy = fabs(data.ball_vy);
                data.score2++;
            }
        }

        send_data();

        BeginDrawing();
        ClearBackground(DARKBLUE);

        // --- 在这里可以插入你的砖块绘制 ---
        // --- 在这里可以插入你的粒子特效 ---
        // --- 在这里可以插入你的道具 ---

        DrawRectangle(data.paddle1_x, SCREEN_HEIGHT - 40, PADDLE_WIDTH, PADDLE_HEIGHT, BLUE);
        DrawRectangle(data.paddle2_x, 25, PADDLE_WIDTH, PADDLE_HEIGHT, RED);
        DrawCircle(data.ball_x, data.ball_y, BALL_RADIUS, YELLOW);

        DrawText(TextFormat("P1 Score: %d", data.score1), 10, 10, 20, WHITE);
        DrawText(TextFormat("P2 Score: %d", data.score2), 10, 35, 20, WHITE);

        EndDrawing();
    }

    close(sock);
    CloseWindow();
    return 0;
}