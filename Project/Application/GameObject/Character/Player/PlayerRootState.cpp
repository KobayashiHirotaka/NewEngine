/**
 * @file PlayerRootState.cpp
 * @brief Playerの通常状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerRootState.h"
#include "PlayerJumpState.h"
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
	Move();

	Jump();
}

void PlayerRootState::Move()
{
	//コントローラーの取得
	if (input_->GetJoystickState())
	{
		//入力
		const float kValueY = -0.3f;

		//アニメーション
		const int kAnimationBackMove = 0;
		const int kAnimationFlontMove = 1;
		const int kAnimationGuard = 2;
		const int kAnimationIdle = 5;
		const float animationSpeed = 1.5f;

		//速度
		const float kMoveSpeed = 0.04f;

		Vector3 velocity = {(float)input_->GetLeftStickX(), 0.0f, 0.0f };

		//敵の位置を取得する
		Vector3 enemyPosition = player_->GetEnemy()->GetWorldPosition();

		if (player_->GetCharacterState().isHitCharacter && !player_->GetAttackData().isAttack)
		{
			const float kPushSpeed = 0.05f;
			if (player_->GetCharacterState().direction == Direction::Right && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > input_->GetDeadZone()))
			{
				//敵を右方向に押す
				PushEnemy(enemyPosition, kPushSpeed);
			}
			else if (player_->GetCharacterState().direction == Direction::Left && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -input_->GetDeadZone()))
			{
				//敵を左方向に押す
				PushEnemy(enemyPosition, -kPushSpeed);
			}
		}

		//移動処理
		//前方向に移動(右を向いている場合)
		if (player_->GetCharacterState().direction == Direction::Right && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > input_->GetDeadZone()) && !player_->GetCharacterState().isDown)
		{
			//速度の設定
			velocity.x = kMoveSpeed;
			moveDirection_ = Flont;
			player_->SetIsGuard(false);
		}

		//前方向に移動(左を向いている場合)
		if (player_->GetCharacterState().direction == Direction::Left && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -input_->GetDeadZone()) && !player_->GetCharacterState().isDown)
		{
			//速度の設定
			velocity.x = -kMoveSpeed;
			moveDirection_ = Flont;
			player_->SetIsGuard(false);
		}

		//後ろ方向に移動(右を向いている場合)
		if (player_->GetCharacterState().direction == Direction::Right && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -input_->GetDeadZone()) && !player_->GetCharacterState().isDown)
		{
			player_->SetIsGuard(true);

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !(input_->GetLeftStickY() < kValueY))
			{
				//速度の設定
				velocity.x = -kMoveSpeed;
				moveDirection_ = Back;
			}

			//止まってガード
			if (player_->GetCharacterState().isGuard && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) || input_->GetLeftStickY() < kValueY))
			{
				//アニメーション
				animationIndex_ = kAnimationGuard;
				player_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, player_->GetModel());

				//速度の設定
				velocity.x = 0.0f;
				moveDirection_ = Default;
			}
		}

		//後ろ方向に移動(左を向いている場合)
		if (player_->GetCharacterState().direction == Direction::Left && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > input_->GetDeadZone()) && !player_->GetCharacterState().isDown)
		{
			player_->SetIsGuard(true);

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !(input_->GetLeftStickY() < kValueY))
			{
				//速度の設定
				velocity.x = kMoveSpeed;
				moveDirection_ = Back;
			}

			//止まってガード
			if (player_->GetCharacterState().isGuard && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) || input_->GetLeftStickY() < kValueY))
			{
				//アニメーション
				animationIndex_ = kAnimationGuard;
				player_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, player_->GetModel());

				//速度の設定
				velocity.x = 0.0f;
				moveDirection_ = Default;
			}
		}

		//移動していない場合
		if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) &&
			!(input_->GetLeftStickX() > input_->GetDeadZone()) && !(input_->GetLeftStickX() < -input_->GetDeadZone()))
		{
			velocity.x = 0.0f;
			moveDirection_ = Default;
			player_->SetIsGuard(false);
		}

		//移動
		if (moveDirection_ == Flont)
		{
			//アニメーション
			animationIndex_ = kAnimationFlontMove;
			player_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, player_->GetModel());

			//移動処理
			velocity = Normalize(velocity);
			velocity = Multiply(kMoveSpeed, velocity);
			player_->Move(velocity);

		}
		else if (moveDirection_ == Back)
		{
			//アニメーション
			animationIndex_ = kAnimationBackMove;
			player_->UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, player_->GetModel());

			//移動処理
			velocity = Normalize(velocity);
			velocity = Multiply(kMoveSpeed, velocity);
			player_->Move(velocity);
		}
		else if(!player_->GetCharacterState().isGuard)
		{
			//アニメーション
			const float kIdleAnimationSpeed = 1.0f;

			animationIndex_ = kAnimationIdle;
			player_->UpdateAnimationTime(animationTime_, true, kIdleAnimationSpeed, animationIndex_, player_->GetModel());
		}
	}

	player_->SetAnimationIndex(animationIndex_);
}

void PlayerRootState::Jump()
{
	//ジャンプ
	if ((input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) || input_->GetLeftStickY() > input_->GetDeadZone()) && player_->GetWorldTransform().translation.y <= 0.0f)
	{
		player_->ChangeState(std::make_unique<PlayerJumpState>());
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