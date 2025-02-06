/**
 * @file PlayerRootState.cpp
 * @brief Playerの通常状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerRootState.h"
#include "PlayerJumpState.h"
#include "PlayerAttackState.h"
#include "Player.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"

void PlayerRootState::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	const int kAnimationIdle = 5;
	animationIndex_ = kAnimationIdle;
	player_->SetAnimationIndex(animationIndex_);
}

void PlayerRootState::Update()
{
    //移動
	Move();

    //ジャンプ
	Jump();

    //攻撃
    Attack();
}

void PlayerRootState::Move()
{
    //コントローラーの取得
    if (input_->GetJoystickState()) 
    {
        //定数
        const float kValueY = -0.3f;
        const float kPushSpeed = 0.05f;
        const float kIdleAnimationSpeed = 1.0f;
        const float kMoveAnimationSpeed = 1.5f;
        float animationSpeed = kMoveAnimationSpeed;

        //アニメーション定数
        enum AnimationIndex 
        {
            kAnimationBackMove = 0,
            kAnimationFlontMove = 1,
            kAnimationGuard = 2,
            kAnimationIdle = 5
        };

        //入力用の変数
        bool isMovingRight = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > input_->GetDeadZone());
        bool isMovingLeft = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -input_->GetDeadZone());
        bool isGuarding = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP)) || input_->GetLeftStickY() < kValueY;

        // 速度設定
        Vector3 velocity = { (float)input_->GetLeftStickX(), 0.0f, 0.0f };
        Vector3 enemyPosition = player_->GetEnemy()->GetWorldPosition();

        // ヒット時の敵の押し処理
        if (player_->GetCharacterState().isHitCharacter && !player_->GetAttackData().isAttack) 
        {
            Direction direction = player_->GetCharacterState().direction;

            if (direction == Direction::Right && isMovingRight || direction == Direction::Left && isMovingLeft)
            {
                PushEnemy(enemyPosition, (direction == Direction::Right) ? kPushSpeed : -kPushSpeed);
            }
        }

        //移動方向の設定
        if (!player_->GetIsDown())
        {
            if (player_->GetCharacterState().direction == Direction::Right) 
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
            player_->SetIsGuard(true);

            if (isGuarding) 
            {
                animationIndex_ = kAnimationGuard;
                moveDirection_ = Default;
            }
        }
        else 
        {
            player_->SetIsGuard(false);
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
            player_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, player_->GetModel());
        }
        else if (moveDirection_ == Back)
        {
            animationIndex_ = kAnimationBackMove;
            animationSpeed = kMoveAnimationSpeed;
            player_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, player_->GetModel());
        }
        else if (!player_->GetCharacterState().isGuard)
        {
            animationIndex_ = kAnimationIdle;
            animationSpeed = kIdleAnimationSpeed;
            player_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, player_->GetModel());
        }

        //移動処理
        velocity.x *= moveSpeed[moveDirection_];
        velocity = Normalize(velocity);
        velocity = Multiply(moveSpeed[moveDirection_], velocity);
        player_->Move(velocity);
    }

    player_->SetAnimationIndex(animationIndex_);
}

void PlayerRootState::Jump()
{
    //コントローラーの取得
    if (input_->GetJoystickState())
    {
        //ジャンプ
        if ((input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) || input_->GetLeftStickY() > input_->GetDeadZone()) && player_->GetWorldTransform().translation.y <= 0.0f)
        {
            player_->ChangeState(std::make_unique<PlayerJumpState>());
        }
    }
}

void PlayerRootState::Attack()
{
    bool isMovingRight = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > input_->GetDeadZone());
    bool isMovingLeft = (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -input_->GetDeadZone());

    //コントローラーの取得
    if (input_->GetJoystickState())
    {
        //攻撃
        //弱攻撃
        if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
        {
            player_->SetAttackType("弱攻撃");
            player_->StartAttack(player_->GetAttackData().isLightPunch);
            player_->ChangeState(std::make_unique<PlayerAttackState>());
        }
        //弾攻撃
        else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
        {
            player_->SetAttackType("ショット");
            player_->StartAttack(player_->GetAttackData().isShot);
            player_->ChangeState(std::make_unique<PlayerAttackState>());
        }
        //特殊技
        else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
        {
            //タックル攻撃
            if (isMovingRight || !isMovingLeft)
            {
                player_->SetAttackType("タックル");
                player_->StartAttack(player_->GetAttackData().isTackle);
                player_->ChangeState(std::make_unique<PlayerAttackState>());
            }
        }
    }
}


void PlayerRootState::PushEnemy(Vector3& enemyPosition, float pushSpeed)
{
	//敵の位置を取得する
	WorldTransform enemyWorldTransform = player_->GetEnemy()->GetWorldTransform();

	//敵の位置を押す速度分だけ更新
	enemyPosition.x += pushSpeed;

	//敵のワールドトランスフォームを更新
	enemyWorldTransform.translation = enemyPosition;
	enemyWorldTransform.UpdateMatrixEuler();
}