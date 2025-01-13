/**
 * @file AttackEditor.h
 * @brief 攻撃に関するパラメータ調整ができるエディター
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Externals/nlohmann/json.hpp"
#include "Application/GameObject/Character/Direction.h"
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

    //当たり判定
    Vector3 rightCollisionMin = { 0.0f, 0.0f, 0.0f };
    Vector3 rightCollisionMax = { 0.0f, 0.0f, 0.0f };
    Vector3 leftCollisionMin = { 0.0f, 0.0f, 0.0f };
    Vector3 leftCollisionMax = { 0.0f, 0.0f, 0.0f };

    //攻撃の属性
    //std::string attackAttribute;
};

class AttackEditor
{
public:
    /// <summary>インスタンスの取得</summary>
    static AttackEditor* GetInstance();

    /// <summary>初期化</summary>
    void Initialize();

    /// <summary>更新</summary>
    void Update();

    /// <summary>調整したパラメータをセット</summary>
    void SetAttackParameters(const std::string& name, int& attackStartTime, int& attackEndTime, int& recoveryTime, int& damage,
        float& guardGaugeIncreaseAmount, float& finisherGaugeIncreaseAmount, float& hitStop, AABB& collision, bool isPlayer, 
        Direction& direction);

    /// <summary>ファイルをセーブ</summary>
    void SaveFile(const std::string& saveFilePath, const std::unordered_map<std::string, AttackParameter>& attackParameters);

    /// <summary>ファイルをロード</summary>
    void LoadFile(const std::string& loadFilePath, std::unordered_map<std::string, AttackParameter>& attackParameters);

private:
    //シングルトン
    AttackEditor() = default;
    ~AttackEditor() = default;
    AttackEditor(const AttackEditor& obj) = default;
    AttackEditor& operator=(const AttackEditor& obj) = default;

    //日本語フォントのロード
    void LoadJapaneseFont();

private:
    //プレイヤーのパラメータ
    std::unordered_map<std::string, AttackParameter> playerAttackParameter_;

    //エネミーのパラメータ
    std::unordered_map<std::string, AttackParameter> enemyAttackParameter_;

    //タブの名前
    std::string tempTabName_;

    //プレイヤーのファイルパス
    std::string savePlayerFilePath_ = "Resource/AttackData/AttackPlayerData.json";
    std::string loadPlayerFilePath_ = "Resource/AttackData/AttackPlayerData.json";

    //エネミーのファイルパス
    std::string saveEnemyFilePath_ = "Resource/AttackData/AttackEnemyData.json";
    std::string loadEnemyFilePath_ = "Resource/AttackData/AttackEnemyData.json";


    //ImGui用の変数
    const int kIntMinValue_ = 0;
    const int kMaxAttackTime_ = 60;
    const int kMaxRecoveryTime_ = 100;
    const int kMaxDamage_ = 100;
    const float kFloatMinValue_ = 0.0f;
    const float kMaxGuardGauge_ = 50.0f;
    const float kMaxFinisherGauge_ = 50.0f;
    const float kMaxHitStop_ = 1.0f;
    const float kMinCollision_ = -3.0f;
    const float kMaxCollision_ = 3.0f;
};