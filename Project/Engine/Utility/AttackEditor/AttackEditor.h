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
    //当たり判定のつき始めの時間
    int attackStartTime = 0;

    //当たり判定のつき終わりの時間
    int attackEndTime = 0;

    //攻撃後の硬直時間
    int recoveryTime = 0;

    //ダメージ
    int damage = 0;

    //ガードゲージ増加量
    float guardGaugeIncreaseAmount = 0.0f;

    //必殺技ゲージ増加量
    float finisherGaugeIncreaseAmount = 0.0f;

    //ヒットストップ
    float hitStop = 0.0f;

    //攻撃の属性
    //std::string attackAttribute;
};

class AttackEditor
{
public:
    static AttackEditor* GetInstance();

    void Initialize();

    void Update();

    void LoadJapaneseFont();

    void SaveFile(const std::string& saveFilePath, const std::unordered_map<std::string, AttackParameter>& attackParameters);

    void LoadFile(const std::string& loadFilePath, std::unordered_map<std::string, AttackParameter>& attackParameters);

    void SetAttackParameters(const std::string& name, int& attackStartTime, int& attackEndTime, int& recoveryTime, 
        int& damage, float& guardGaugeIncreaseAmount, float& finisherGaugeIncreaseAmount, float& hitStop, bool isPlayer);

private:
    AttackEditor() = default;
    ~AttackEditor() = default;
    AttackEditor(const AttackEditor& obj) = default;
    AttackEditor& operator=(const AttackEditor& obj) = default;

private:
    std::unordered_map<std::string, AttackParameter> playerAttackParameter_;
    std::unordered_map<std::string, AttackParameter> enemyAttackParameter_;

    //順序を保持
    std::vector<std::string> playerAttackOrder_;
    std::vector<std::string> enemyAttackOrder_;

    std::string tempTabName_;

    std::string savePlayerFilePath_ = "resource/AttackData/AttackPlayerData.json";
    std::string loadPlayerFilePath_ = "resource/AttackData/AttackPlayerData.json";

    std::string saveEnemyFilePath_ = "resource/AttackData/AttackEnemyData.json";
    std::string loadEnemyFilePath_ = "resource/AttackData/AttackEnemyData.json";
};