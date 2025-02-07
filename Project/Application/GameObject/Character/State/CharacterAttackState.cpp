/**
 * @file CharacterAttackState.cpp
 * @brief 各キャラクターの攻撃状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "CharacterAttackState.h"
#include "Application/GameObject/Character/BaseCharacter.h"

void CharacterAttackState::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	direction_ = baseCharacter_->GetCharacterState().direction;

	attackTable_ = {
		{"弱攻撃", {kLightPunchAnimationIndex_, kLightPunchAnimationSpeed_, kLightPunchMoveData_, {"中攻撃(ターゲット)", baseCharacter_->GetAttackData().isLightPunch, baseCharacter_->GetAttackData().isTCMiddlePunch, true}}},
		{"中攻撃(ターゲット)", {kTCMiddlePunchAnimationIndex_, kTCMiddlePunchAnimationSpeed_, kTCMiddlePunchMoveData_, {"強攻撃", baseCharacter_->GetAttackData().isTCMiddlePunch, baseCharacter_->GetAttackData().isHighPunch, true}}},
		{"強攻撃", {kHighPunchAnimationIndex_, kHighPunchAnimationSpeed_, kHighPunchMoveData_, {"", baseCharacter_->GetAttackData().isHighPunch, false, false}}},
		{"ショット", {kShotAnimationIndex_, kShotAnimationSpeed_, kShotMoveData_, {"", baseCharacter_->GetAttackData().isShot, false, false}}}
	};
}

void CharacterAttackState::Update()
{
	if (baseCharacter_->GetAttackType() == "弱攻撃" || baseCharacter_->GetAttackType() == "中攻撃(ターゲット)" || baseCharacter_->GetAttackType() == "強攻撃" ||
		baseCharacter_->GetAttackType() == "ショット")
	{
		HandleAttack();
	}
}

void CharacterAttackState::Attack(const int kAnimationIndex, const float animationSpeed, const AttackMoveData attackMoveData, const AttackCancelData attackCancelData)
{
	//アニメーション
	animationIndex_ = kAnimationIndex;

	if (!baseCharacter_->GetIsDown())
	{
		baseCharacter_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, baseCharacter_->GetModel());
	}

	//攻撃判定をつけるタイミングの設定
	baseCharacter_->EvaluateAttackTiming(attackAnimationFrame_);

	//アニメーションの設定
	baseCharacter_->SetAnimationIndex(animationIndex_);

	if (attackMoveData.isAttackMove)
	{
		AttackMove(attackMoveData);
	}

	//終了処理
	if (attackAnimationFrame_ > baseCharacter_->GetAttackData().recoveryTime)
	{
		EndAttack();
	}

	//キャンセル処理
	if (attackCancelData.isAttackCancel && attackAnimationFrame_ >= baseCharacter_->GetAttackData().cancelStartTime && attackAnimationFrame_ < baseCharacter_->GetAttackData().cancelEndTime)
	{
		if (baseCharacter_->GetTag() == "Player" && (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)))
		{
			Cancel(attackCancelData_);
		}
		else if (baseCharacter_->GetTag() == "Enemy" && baseCharacter_->GetIsDown())
		{
			Cancel(attackCancelData_);
		}
	}

	//アニメーションフレームの加算
	attackAnimationFrame_ += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
}

void CharacterAttackState::Shot(const Vector3 position)
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

		baseCharacter_->ShootBullet(bulletStartPosition, bulletVelocity);

		hasShot_ = true;
	}

	//終了処理
	const int kShotEndTimer = 50;
	if (attackAnimationFrame_ >= kShotEndTimer)
	{
		hasShot_ = false;
	}
}

void CharacterAttackState::AttackMove(const AttackMoveData attackMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (attackAnimationFrame_ < attackMoveData.moveTime)
	{
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction_ == Direction::Right) ? attackMoveData.moveSpeed : -attackMoveData.moveSpeed;
		baseCharacter_->Move(velocity);
	}
}

void CharacterAttackState::Cancel(AttackCancelData attackCancelData)
{
	baseCharacter_->SetAttackType(attackCancelData.attackType);
	baseCharacter_->SetIsAttack(false);
	attackCancelData.isCurrentAttack = false;
	attackCancelData.isNextAttack = true;
	animationTime_ = 0.0f;
	attackAnimationFrame_ = 0;
	baseCharacter_->GetModel()->SetAnimationTime(animationTime_);
	baseCharacter_->ResetCollision();
}

void CharacterAttackState::HandleAttack()
{
	std::string attackType = baseCharacter_->GetAttackType();

	if (attackTable_.find(attackType) != attackTable_.end())
	{
		const AttackData& data = attackTable_[attackType];

		//設定適用
		attackMoveData_ = data.moveData;
		attackCancelData_ = data.cancelData;

		//攻撃
		Attack(data.animationIndex, data.animationSpeed, attackMoveData_, attackCancelData_);

		//弾攻撃の場合
		if (attackType == "ショット")
		{
			Shot(baseCharacter_->GetWorldPosition());
		}
	}
}
