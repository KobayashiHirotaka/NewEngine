/**
 * @file PlayerDownState.cpp
 * @brief Playerのダウン状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerDownState.h"
#include "Application/GameObject/Character/Player/Player.h"

void PlayerDownState::Initialize()
{
	//向き
	direction_ = player_->GetCharacterState().direction;

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//ダウンの種類ごとのデータを格納
	downTable_ = {
		{"軽ダウン", {kAnimationLightDown, lightDownAnimationSpeed_, kLightMoveData_, kLightParticleData_}}
	};
}

void PlayerDownState::Update()
{
	if (player_->GetCharacterState().isHitLightPunch || player_->GetCharacterState().isHitTCMiddlePunch || player_->GetCharacterState().isHitHighPunch ||
		player_->GetCharacterState().isHitBullet)
	{
		HandleDown();
	}

	//ParticleEffectPlayerの更新
	particleEffectPlayer_->Update();
}

void PlayerDownState::Down(const int kAnimationIndex, const float animationSpeed, const DownParticleData downParticleData, const DownMoveData downMoveData)
{
	//ダウン状態に設定
	player_->SetIsDown(true);
	downAnimationTimer_--;

	animationIndex_ = kAnimationIndex;

	player_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, player_->GetModel());

	//アニメーションの設定
	player_->SetAnimationIndex(animationIndex_);

	//パーティクル
	if (downAnimationTimer_ > downParticleData.particleTime)
	{
		particleEffectPlayer_->PlayParticle("Hit", { player_->GetWorldPosition().x + downParticleData.particlePosition.x,
			player_->GetWorldPosition().y + downParticleData.particlePosition.y, player_->GetWorldPosition().z });
	}

	if (downMoveData.isDownMove)
	{
		DownMove(downMoveData);
	}

	if (downAnimationTimer_ < 0 && player_->GetBaseData().hp_ < 0)
	{
		//アニメーションの設定
		const int kAnimationIdle = 5;
		player_->EndDownAnimation(kAnimationIdle);
		player_->ChangeState(std::make_unique<PlayerRootState>());
	}
}

void PlayerDownState::DownMove(const DownMoveData downMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (downAnimationTimer_ < downMoveData.moveTime)
	{
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction_ == Direction::Right) ? downMoveData.moveSpeed : -downMoveData.moveSpeed;
		player_->Move(velocity);
	}
}

void PlayerDownState::HandleDown()
{
	std::string downType = player_->GetDownType();

	if (downTable_.find(downType) != downTable_.end())
	{
		const DownData& data = downTable_[downType];

		// ダウン処理を実行
		Down(data.animationIndex, data.animationSpeed, data.particleData, data.moveData);
	}
}