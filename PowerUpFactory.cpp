#include "PowerUpFactory.h"
#include <fstream>
#include <stdexcept>

json PowerUpFactory::config;
std::mt19937 PowerUpFactory::rng(std::random_device{}());
std::uniform_real_distribution<float> PowerUpFactory::dist(0.0f, 1.0f);

void PowerUpFactory::LoadConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("无法打开配置文件: " + path);
    file >> config;
}

PowerUp* PowerUpFactory::CreatePowerUp(PowerUpType type, Vector2 position) {
    for (auto& item : config["powerUps"]) {
        if ((std::string)item["type"] == PowerUpTypeToString(type)) {
            return new PowerUp(
                type,
                position,
                item["speed"],
                item["duration"],
                Color{item["color"][0], item["color"][1], item["color"][2], item["color"][3]},
                item.contains("width") ? item["width"] : 100,
                item.contains("height") ? item["height"] : 20,
                item.contains("speedMultiplier") ? item["speedMultiplier"] : 1.0f
            );
        }
    }
    return nullptr;
}

PowerUp* PowerUpFactory::CreateRandomPowerUp(Vector2 position) {
    float totalProb = 0.0f;
    float r = dist(rng);
    for (auto& item : config["powerUps"]) {
        float p = item["spawnProbability"];
        if (r < p + totalProb) {
            PowerUpType type = StringToPowerUpType((std::string)item["type"]);
            return CreatePowerUp(type, position);
        }
        totalProb += p;
    }
    return nullptr;
}

// 辅助函数（需实现）
std::string PowerUpTypeToString(PowerUpType t) {
    switch(t) {
        case PowerUpType::LENGTHEN_PADDLE: return "LENGTHEN_PADDLE";
        case PowerUpType::MULTI_BALL: return "MULTI_BALL";
        case PowerUpType::SLOW_BALL: return "SLOW_BALL";
        default: return "NONE";
    }
}

PowerUpType StringToPowerUpType(const std::string& s) {
    if (s == "LENGTHEN_PADDLE") return PowerUpType::LENGTHEN_PADDLE;
    if (s == "MULTI_BALL") return PowerUpType::MULTI_BALL;
    if (s == "SLOW_BALL") return PowerUpType::SLOW_BALL;
    return PowerUpType::NONE;
}