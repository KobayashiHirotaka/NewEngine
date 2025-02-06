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
    //コントローラーの取得
    if (input_->GetJoystickState())
    {
        //定数
        const float kValueY = -0.3f;
        const float kIdleAnimationSpeed = 1.0f;
        const float kMoveAnimationSpeed = 1.5f;
        float animationSpeed = kMoveAnimationSpeed;

        //入力用の変数
        bool isMovingRight = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > input_->GetDeadZone());
        bool isMovingLeft = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -input_->GetDeadZone());
        bool isGuarding = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP)) || input_->GetLeftStickY() < kValueY;

        // 速度設定
        Vector3 velocity = { (float)input_->GetLeftStickX(), 0.0f, 0.0f };
        Vector3 enemyPosition = enemy_->GetPlayer()->GetWorldPosition();

        //移動方向の設定
        if (!enemy_->GetIsDown())
        {
            if (enemy_->GetCharacterState().direction == Direction::Right)
            {
                moveDirection_ = isMovingRight ? Flont : isMovingLeft ? Back : Default;
            }
            else
            {
                moveDirection_ = isMovingLeft ? Flont : isMovingRight ? Back : Default;
            }
        }

        //ガード処理
        if (moveDirection_ == Back)
        {
            if (isGuarding)
            {
                animationIndex_ = kAnimationGuard;
                moveDirection_ = Default;
            }
        }

        //停止時の処理
        if (!isMovingRight && !isMovingLeft)
        {
            moveDirection_ = Default;
        }

        //アニメーション設定
        if (moveDirection_ == Flont)
        {
            animationIndex_ = kAnimationFlontMove;
            animationSpeed = kMoveAnimationSpeed;
            enemy_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, enemy_->GetModel());
        }
        else if (moveDirection_ == Back)
        {
            animationIndex_ = kAnimationBackMove;
            animationSpeed = kMoveAnimationSpeed;
            enemy_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, enemy_->GetModel());
        }
        else if (!enemy_->GetCharacterState().isGuard)
        {
            animationIndex_ = kAnimationIdle;
            animationSpeed = kIdleAnimationSpeed;
            enemy_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, enemy_->GetModel());
        }

        //移動処理
        velocity.x *= moveSpeed[moveDirection_];
        velocity = Normalize(velocity);
        velocity = Multiply(moveSpeed[moveDirection_], velocity);
        enemy_->Move(velocity);
    }

    enemy_->SetAnimationIndex(animationIndex_);
}

void EnemyRootState::Jump()
{
    //コントローラーの取得
    if (input_->GetJoystickState())
    {
        //ジャンプ
        if ((input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) || input_->GetLeftStickY() > input_->GetDeadZone()) && enemy_->GetWorldTransform().translation.y <= 0.0f)
        {
            enemy_->ChangeState(std::make_unique<EnemyJumpState>());
        }
    }
}

void EnemyRootState::Attack()
{
    //弾攻撃
    if (patternCount_ == kPatternCount_[4] && !enemy_->GetCharacterState().isDown)
    {
        enemy_->SetAttackType("ショット");
        enemy_->StartAttack(enemy_->GetAttackData().isShot);
        enemy_->ChangeState(std::make_unique<EnemyAttackState>());
    }
    //弱攻撃
    else if (patternCount_ == kPatternCount_[5] && !enemy_->GetCharacterState().isDown)
    {
        enemy_->SetAttackType("弱攻撃");
        enemy_->StartAttack(enemy_->GetAttackData().isLightPunch);
        enemy_->ChangeState(std::make_unique<EnemyAttackState>());
    }
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
