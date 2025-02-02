/**
 * @file PlayerJumpState.cpp
 * @brief Playerのジャンプ状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerJumpState.h"
#include "PlayerRootState.h"
#include "Player.h"

void PlayerJumpState::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	//ジャンプの速さ
	const float kJumpFirstSpeed_ = 18.0f;

	velocity_ = { 0.0f, kJumpFirstSpeed_, 0.0f };

	if (input_->GetJoystickState())
	{
		//スティック用のパラメータ
		float joystickInput = input_->GetLeftStickX();

		const float kStickThreshold = 0.3f;
		const float kMoveSpeedX = 3.0f;

		//入力に応じたジャンプの初期設定
		if (fabs(joystickInput) > kStickThreshold)
		{
			velocity_.x = (joystickInput > 0 ? kMoveSpeedX : -kMoveSpeedX);
		}
		else
		{
			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
			{
				velocity_.x = kMoveSpeedX;
			}
			else if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				velocity_.x = -kMoveSpeedX;
			}
		}
	}
}

void PlayerJumpState::Update()
{
	//離れているとき
	if (player_->GetDistance() >= player_->GetMaxDistance())
	{
		velocity_.x = 0.0f;
	}

	//ジャンプ
	Vector3 deltaVelocity = { velocity_.x * GameTimer::GetDeltaTime(),
		velocity_.y * GameTimer::GetDeltaTime(),
		velocity_.z* GameTimer::GetDeltaTime() };

	player_->Jump(deltaVelocity);

	const float kGravityAcceleration_ = 80.0f;

	Vector3 accelerationVector_ = { 0.0f, -kGravityAcceleration_ * GameTimer::GetDeltaTime(),0.0f };

	velocity_ = Add(velocity_, accelerationVector_);

	//アニメーション
	const int kAnimationIdle = 5;
	const float animationSpeed = 1.0f;

	animationIndex_ = kAnimationIdle;
	player_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, player_->GetModel());

	player_->SetAnimationIndex(animationIndex_);

	//終了処理
	if (player_->GetWorldTransform().translation.y <= 0.0f || player_->GetCharacterState().isDown)
	{
		player_->SetPositionY(0.0f);
		animationTime_ = 0.0f;
		player_->GetModel()->SetAnimationTime(animationTime_);

		player_->ChangeState(std::make_unique<PlayerRootState>());
	}
}