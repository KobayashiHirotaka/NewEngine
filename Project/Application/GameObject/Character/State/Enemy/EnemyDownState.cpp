/**
 * @file EnemyDownState.cpp
 * @brief 敵のダウン状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EnemyDownState.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"

void EnemyDownState::Initialize()
{
	//向き
	direction_ = enemy_->GetCharacterState().direction;

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//ダウンの種類ごとのデータを格納
	downTable_ = {
		{"軽ダウン", {kAnimationLightDown, lightDownAnimationSpeed_, kLightMoveData_, kLightParticleData_}}
	};
}

void EnemyDownState::Update()
{
	if (enemy_->GetCharacterState().isHitLightPunch || enemy_->GetCharacterState().isHitTCMiddlePunch || enemy_->GetCharacterState().isHitHighPunch ||
		enemy_->GetCharacterState().isHitBullet)
	{
		HandleDown();
	}

	//ParticleEffectPlayerの更新
	particleEffectPlayer_->Update();
}

void EnemyDownState::Down(const int kAnimationIndex, const float animationSpeed, const DownParticleData downParticleData, const DownMoveData downMoveData)
{
	//ダウン状態に設定
	enemy_->SetIsDown(true);
	downAnimationTimer_--;

	animationIndex_ = kAnimationIndex;

	enemy_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, enemy_->GetModel());

	//アニメーションの設定
	enemy_->SetAnimationIndex(animationIndex_);

	//パーティクル
	if (downAnimationTimer_ > downParticleData.particleTime)
	{
		particleEffectPlayer_->PlayParticle("Hit", { enemy_->GetWorldPosition().x + downParticleData.particlePosition.x,
			enemy_->GetWorldPosition().y + downParticleData.particlePosition.y, enemy_->GetWorldPosition().z });
	}

	if (downMoveData.isDownMove)
	{
		DownMove(downMoveData);
	}

	if (downAnimationTimer_ < 0 && enemy_->GetBaseData().hp_ > 0)
	{
		//アニメーションの設定
		const int kAnimationIdle = 5;
		enemy_->EndDownAnimation(kAnimationIdle);
		enemy_->ChangeState(std::make_unique<EnemyRootState>());
	}
}

void EnemyDownState::DownMove(const DownMoveData downMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (downAnimationTimer_ < downMoveData.moveTime)
	{
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction_ == Direction::Right) ? downMoveData.moveSpeed : -downMoveData.moveSpeed;
		enemy_->Move(velocity);
	}
}

void EnemyDownState::HandleDown()
{
	std::string downType = enemy_->GetDownType();

	if (downTable_.find(downType) != downTable_.end())
	{
		const DownData& data = downTable_[downType];

		// ダウン処理を実行
		Down(data.animationIndex, data.animationSpeed, data.particleData, data.moveData);
	}
}
