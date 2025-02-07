/**
 * @file EnemyRootState.cpp
 * @brief 敵の通常状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EnemyRootState.h"
#include "EnemyJumpState.h"
#include "EnemyAttackState.h"
#include "EnemyDownState.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/GameObject/Character/Player/Player.h"

void EnemyRootState::Initialize()
{
    //Inputのインスタンスを取得
    input_ = Engine::Input::GetInstance();

    const int kAnimationIdle = 5;
    animationIndex_ = kAnimationIdle;
    enemy_->SetAnimationIndex(animationIndex_);

    //行動パターンの設定
    patternCount_ = RandomMove();
}

void EnemyRootState::Update()
{
    //移動
    Move();

    //ジャンプ
    Jump();

    //攻撃
    Attack();

    if (input_->PushKey(DIK_RETURN))
    {
        enemy_->SetDownType("軽ダウン");
        enemy_->SetIsLightPunch(true);
        enemy_->ChangeState(std::make_unique<EnemyDownState>());
    }
}

void EnemyRootState::Move()
{
  
}

void EnemyRootState::Jump()
{
    
}

void EnemyRootState::Attack()
{
    
}

int EnemyRootState::Random(int min_value, int max_value)
{
    //乱数を生成
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min_value, max_value);

    return dis(gen);
}

int EnemyRootState::RandomMove()
{
    std::vector<int> actions;

    //前歩きか後ろ歩きをランダムで設定
    actions = { kPatternCount_[1],kPatternCount_[2], kPatternCount_[2], kPatternCount_[6] };

    //乱数を生成
    const int kIndexOffset = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, static_cast<int>(actions.size()) - kIndexOffset);

    return actions[dis(gen)];
}

int EnemyRootState::RandomAttackOrMove()
{
    std::vector<int> actions;

    //体力の半分
    const int kDivisionFactor = 2;
    const int kHalfHP = enemy_->GetBaseData().kMaxHp_ / kDivisionFactor;

    //体力に応じて行動をランダムで設定
    if (enemy_->GetBaseData().hp_ >= kHalfHP)
    {
        //前歩きか突進攻撃
        actions = { kPatternCount_[1], kPatternCount_[2], kPatternCount_[3], kPatternCount_[6] };
    }
    else
    {
        //前歩きか後ろ歩きか弾攻撃
        actions = { kPatternCount_[1], kPatternCount_[2], kPatternCount_[4], kPatternCount_[6] };
    }

    //乱数を生成
    const int kIndexOffset = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, static_cast<int>(actions.size()) - kIndexOffset);

    return actions[dis(gen)];
}

int EnemyRootState::RandomBulletOrMove()
{
    std::vector<int> actions;

    //前歩きか後ろ歩きか弾攻撃
    actions = { kPatternCount_[1], kPatternCount_[2], kPatternCount_[4], kPatternCount_[6] };

    //乱数を生成
    const int kIndexOffset = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, static_cast<int>(actions.size()) - kIndexOffset);

    return actions[dis(gen)];
}
