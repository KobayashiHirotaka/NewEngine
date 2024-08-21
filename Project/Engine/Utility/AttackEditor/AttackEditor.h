#pragma once
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/externals/nlohmann/json.hpp"
#include <variant>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>

using json = nlohmann::json;

struct AttackParameter 
{
    int attackStartTime = 0;

    int attackEndTime = 0;

    int recoveryTime = 0;
};

class AttackEditor
{
public:
    static AttackEditor* GetInstance();

    void Initialize();

    void Update();

    void SaveFile(const std::string& saveFilePath, const std::unordered_map<std::string, AttackParameter>& attackParameters);

    void LoadFile(const std::string& loadFilePath, std::unordered_map<std::string, AttackParameter>& attackParameters);

    void SetAttackParameters(const std::string& name, int& attackStartTime, int& attackEndTime, int& recoveryTime, bool isPlayer);

private:
    AttackEditor() = default;
    ~AttackEditor() = default;
    AttackEditor(const AttackEditor& obj) = default;
    AttackEditor& operator=(const AttackEditor& obj) = default;

private:
    std::unordered_map<std::string, AttackParameter> playerAttackParameter_;
    std::unordered_map<std::string, AttackParameter> enemyAttackParameter_;

    std::string tempTabName_;

    std::string savePlayerFilePath_ = "resource/AttackData/AttackPlayerData.json";
    std::string loadPlayerFilePath_ = "resource/AttackData/AttackPlayerData.json";

    std::string saveEnemyFilePath_ = "resource/AttackData/AttackEnemyData.json";
    std::string loadEnemyFilePath_ = "resource/AttackData/AttackEnemyData.json";
};