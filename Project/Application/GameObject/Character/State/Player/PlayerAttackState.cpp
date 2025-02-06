/**
 * @file PlayerAttackState.cpp
 * @brief Playerの攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerAttackState.h"
#include "PlayerRootState.h"
#include "Application/GameObject/Character/Player/Player.h"

void PlayerAttackState::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	attackAnimationFrame_ = player_->GetAttackData().attackAnimationFrame;

	direction_ = player_->GetCharacterState().direction;

	attackTable_ = {
		{"弱攻撃", {kLightPunchAnimationIndex_, kLightPunchAnimationSpeed_, kLightPunchMoveData_, {"中攻撃(ターゲット)", player_->GetAttackData().isLightPunch, player_->GetAttackData().isTCMiddlePunch, true}}},
		{"中攻撃(ターゲット)", {kTCMiddlePunchAnimationIndex_, kTCMiddlePunchAnimationSpeed_, kTCMiddlePunchMoveData_, {"強攻撃", player_->GetAttackData().isTCMiddlePunch, player_->GetAttackData().isHighPunch, true}}},
		{"強攻撃", {kHighPunchAnimationIndex_, kHighPunchAnimationSpeed_, kHighPunchMoveData_, {"", player_->GetAttackData().isHighPunch, false, false}}},
		{"ショット", {kShotAnimationIndex_, kShotAnimationSpeed_, kShotMoveData_, {"", player_->GetAttackData().isShot, false, false}}}
	};
}

void PlayerAttackState::Update()
{
	if (player_->GetAttackType() == "弱攻撃" || player_->GetAttackType() == "中攻撃(ターゲット)" || player_->GetAttackType() == "強攻撃" ||
		player_->GetAttackType() == "ショット")
	{
		HandleAttack();
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
		player_->EndAttack(attackCancelData_.isCurrentAttack);
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

void PlayerAttackState::Shot(const Vector3 position)
{
	if (!hasShot_)
	{
		const Vector2 kRespownPos = { 0.2f, 0.5f };
		const float kBulletSpeed = 0.1f;
		const float kDirectionFactor = 1.0f;

		//向きに応じた方向係数
		float directionFactor = (direction_ == Direction::Right) ? kDirectionFactor : -kDirectionFactor;

		//弾の発射位置と速度を計算
		Vector3 bulletStartPosition = {
			position.x + kRespownPos.x * directionFactor,
			position.y + kRespownPos.y,
			position.z
		};
		Vector3 bulletVelocity = { kBulletSpeed * directionFactor, 0.0f, 0.0f };

		player_->ShootBullet(bulletStartPosition, bulletVelocity);

		hasShot_ = true;
	}

	//終了処理
	const int kShotEndTimer = 50;
	if (attackAnimationFrame_ >= kShotEndTimer)
	{
		hasShot_ = false;
	}
}

void PlayerAttackState::AttackMove(const AttackMoveData attackMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (attackAnimationFrame_ < attackMoveData.moveTime)
	{
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction_ == Direction::Right) ? attackMoveData.moveSpeed : -attackMoveData.moveSpeed;
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

void PlayerAttackState::HandleAttack()
{
	std::string attackType = player_->GetAttackType();

	if (attackTable_.find(attackType) != attackTable_.end())
	{
		const AttackData& data = attackTable_[attackType];

		//設定適用
		attackMoveData_ = data.moveData;
		attackCancelData_ = data.cancelData;

		//攻撃
		Attack(data.animationId, data.animationSpeed, attackMoveData_, attackCancelData_);

		//弾攻撃の場合
		if (attackType == "ショット")
		{
			Shot(player_->GetWorldPosition());
		}
	}
}