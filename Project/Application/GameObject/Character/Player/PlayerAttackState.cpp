/**
 * @file PlayerAttackState.cpp
 * @brief Playerの攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerAttackState.h"
#include "PlayerRootState.h"
#include "Player.h"

void PlayerAttackState::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	attackAnimationFrame_ = player_->GetAttackData().attackAnimationFrame;
}

void PlayerAttackState::Update()
{
	if (player_->GetAttackType() == "弱攻撃")
	{
		//アニメーション用のパラメーターを設定
		const int kAnimationLighhtPunch = 13;
		const float animationSpeed = 1.5f;

		//攻撃中の移動用のパラメーターを設定
		attackMoveData_.isAttackMove = false;

		//攻撃のキャンセル用のパラメーターを設定
		attackCancelData_.isAttackCancel = true;
		attackCancelData_.isCurrentAttack = player_->GetAttackData().isLightPunch;
		attackCancelData_.isNextAttack = player_->GetAttackData().isTCMiddlePunch;
		attackCancelData_.attackType = "中攻撃(ターゲット)";

		//攻撃
		Attack(kAnimationLighhtPunch, animationSpeed, attackMoveData_, attackCancelData_);
	}
	else if (player_->GetAttackType() == "中攻撃(ターゲット)")
	{
		//アニメーション用のパラメーターを設定
		const int kAnimationTCMiddlePunch = 12;
		const float animationSpeed = 1.5f;

		//攻撃中の移動用のパラメーターを設定
		attackMoveData_.moveTime = 5;
		attackMoveData_.moveSpeed = 0.03f;
		attackMoveData_.isAttackMove = true;  

		//攻撃のキャンセル用のパラメーターを設定
		attackCancelData_.isAttackCancel = false;

		//攻撃
		Attack(kAnimationTCMiddlePunch, animationSpeed, attackMoveData_, attackCancelData_);
	}
}

void PlayerAttackState::Attack(const int kAnimationIndex, const float animationSpeed, const AttackMoveData attackMoveData, const AttackCancelData attackCancelData)
{
	//アニメーション
	animationIndex_ = kAnimationIndex;

	if (!player_->GetIsDown())
	{
		player_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, player_->GetModel());
	}

	//攻撃判定をつけるタイミングの設定
	player_->EvaluateAttackTiming(attackAnimationFrame_);

	//アニメーションの設定
	player_->SetAnimationIndex(animationIndex_);

	if (attackMoveData.isAttackMove)
	{
		AttackMove(attackMoveData);
	}

	//終了処理
	if (attackAnimationFrame_ > player_->GetAttackData().recoveryTime)
	{
		player_->EndAttack(player_->GetAttackData().isLightPunch);
		player_->ResetCollision();
		player_->ChangeState(std::make_unique<PlayerRootState>());
	}

	//キャンセル処理
	if (attackCancelData.isAttackCancel && attackAnimationFrame_ >= player_->GetAttackData().cancelStartTime && attackAnimationFrame_ < player_->GetAttackData().cancelEndTime &&
		(input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)))
	{
		Cancel(attackCancelData_);
	}

	//アニメーションフレームの加算
	attackAnimationFrame_ += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
}

void PlayerAttackState::AttackMove(const AttackMoveData attackMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (attackAnimationFrame_ < attackMoveData.moveTime)
	{
		Direction direction = player_->GetCharacterState().direction;
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction == Direction::Right) ? attackMoveData.moveSpeed : -attackMoveData.moveSpeed;
		player_->Move(velocity);
	}
}

void PlayerAttackState::Cancel(AttackCancelData attackCancelData)
{
	player_->SetAttackType(attackCancelData.attackType);
	player_->SetIsAttack(false);
	attackCancelData.isCurrentAttack = false;
	attackCancelData.isNextAttack = true;
	animationTime_ = 0.0f;
	attackAnimationFrame_ = 0;
	player_->GetModel()->SetAnimationTime(animationTime_);
	player_->ResetCollision();
}