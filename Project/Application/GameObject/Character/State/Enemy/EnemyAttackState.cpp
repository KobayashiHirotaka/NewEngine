/**
 * @file EnemyAttackState.cpp
 * @brief 敵の攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EnemyAttackState.h"
#include "EnemyRootState.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"

void EnemyAttackState::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	direction_ = enemy_->GetCharacterState().direction;

	attackTable_ = {
		{"弱攻撃", {kLightPunchAnimationIndex_, kLightPunchAnimationSpeed_, kLightPunchMoveData_, {"中攻撃(ターゲット)", enemy_->GetAttackData().isLightPunch, enemy_->GetAttackData().isTCMiddlePunch, true}}},
		{"中攻撃(ターゲット)", {kTCMiddlePunchAnimationIndex_, kTCMiddlePunchAnimationSpeed_, kTCMiddlePunchMoveData_, {"強攻撃", enemy_->GetAttackData().isTCMiddlePunch, enemy_->GetAttackData().isHighPunch, true}}},
		{"強攻撃", {kHighPunchAnimationIndex_, kHighPunchAnimationSpeed_, kHighPunchMoveData_, {"", enemy_->GetAttackData().isHighPunch, false, false}}},
		{"ショット", {kShotAnimationIndex_, kShotAnimationSpeed_, kShotMoveData_, {"", enemy_->GetAttackData().isShot, false, false}}}
	};
}

void EnemyAttackState::Update()
{
	if (enemy_->GetAttackType() == "弱攻撃" || enemy_->GetAttackType() == "中攻撃(ターゲット)" || enemy_->GetAttackType() == "強攻撃" ||
		enemy_->GetAttackType() == "ショット")
	{
		HandleAttack();
	}
}

void EnemyAttackState::Attack(const int kAnimationIndex, const float animationSpeed, const AttackMoveData attackMoveData, const AttackCancelData attackCancelData)
{
	//アニメーション
	animationIndex_ = kAnimationIndex;

	if (!enemy_->GetIsDown())
	{
		enemy_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, enemy_->GetModel());
	}

	//攻撃判定をつけるタイミングの設定
	enemy_->EvaluateAttackTiming(attackAnimationFrame_);

	//アニメーションの設定
	enemy_->SetAnimationIndex(animationIndex_);

	if (attackMoveData.isAttackMove)
	{
		AttackMove(attackMoveData);
	}

	//終了処理
	if (attackAnimationFrame_ > enemy_->GetAttackData().recoveryTime)
	{
		enemy_->EndAttack(attackCancelData_.isCurrentAttack);
		enemy_->ResetCollision();
		enemy_->ChangeState(std::make_unique<EnemyRootState>());
	}

	//キャンセル処理
	if (attackCancelData.isAttackCancel && attackAnimationFrame_ >= enemy_->GetAttackData().cancelStartTime && attackAnimationFrame_ < enemy_->GetAttackData().cancelEndTime &&
		(input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)))
	{
		Cancel(attackCancelData_);
	}

	//アニメーションフレームの加算
	attackAnimationFrame_ += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
}

void EnemyAttackState::Shot(const Vector3 position)
{
	if (!hasShot_)
	{
		const Vector2 kRespownPos = { 0.2f, 0.5f };
		const float kBulletSpeed = 0.1f;
		const float kDirectionFactor = 1.0f;

		//向きに応じた方向係数
		float directionFactor = (direction_ == Direction::Left) ? kDirectionFactor : -kDirectionFactor;

		//弾の発射位置と速度を計算
		Vector3 bulletStartPosition = {
			position.x + kRespownPos.x * directionFactor,
			position.y + kRespownPos.y,
			position.z
		};
		Vector3 bulletVelocity = { kBulletSpeed * directionFactor, 0.0f, 0.0f };

		enemy_->ShootBullet(bulletStartPosition, bulletVelocity);

		hasShot_ = true;
	}

	//終了処理
	const int kShotEndTimer = 50;
	if (attackAnimationFrame_ >= kShotEndTimer)
	{
		hasShot_ = false;
	}
}

void EnemyAttackState::AttackMove(const AttackMoveData attackMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (attackAnimationFrame_ < attackMoveData.moveTime)
	{
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction_ == Direction::Right) ? attackMoveData.moveSpeed : -attackMoveData.moveSpeed;
		enemy_->Move(velocity);
	}
}

void EnemyAttackState::Cancel(AttackCancelData attackCancelData)
{
	enemy_->SetAttackType(attackCancelData.attackType);
	enemy_->SetIsAttack(false);
	attackCancelData.isCurrentAttack = false;
	attackCancelData.isNextAttack = true;
	animationTime_ = 0.0f;
	attackAnimationFrame_ = 0;
	enemy_->GetModel()->SetAnimationTime(animationTime_);
	enemy_->ResetCollision();
}

void EnemyAttackState::HandleAttack()
{
	std::string attackType = enemy_->GetAttackType();

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
			Shot(enemy_->GetWorldPosition());
		}
	}
}


