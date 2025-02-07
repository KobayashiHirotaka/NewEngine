/**
 * @file CharacterDownState.cpp
 * @brief 各キャラクターのダウン状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "CharacterDownState.h"
#include "Application/GameObject/Character/BaseCharacter.h"

void CharacterDownState::Initialize()
{
	//向き
	direction_ = baseCharacter_->GetCharacterState().direction;

	//ダウンの種類ごとのデータを格納
	downTable_ = {
		{"軽ダウン", {kAnimationLightDown, lightDownAnimationSpeed_, kLightMoveData_, kLightParticleData_}}
	};
}

void CharacterDownState::Update()
{
	if (baseCharacter_->GetCharacterState().isHitLightPunch || baseCharacter_->GetCharacterState().isHitTCMiddlePunch || baseCharacter_->GetCharacterState().isHitHighPunch ||
		baseCharacter_->GetCharacterState().isHitBullet)
	{
		HandleDown();
	}
}

void CharacterDownState::Down(const int kAnimationIndex, const float animationSpeed, const DownParticleData, const DownMoveData downMoveData)
{
	//ダウン状態に設定
	baseCharacter_->SetIsDown(true);
	downAnimationTimer_--;

	animationIndex_ = kAnimationIndex;

	baseCharacter_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, baseCharacter_->GetModel());

	//アニメーションの設定
	baseCharacter_->SetAnimationIndex(animationIndex_);

	////パーティクル
	//if (downAnimationTimer_ > downParticleData.particleTime)
	//{
	//	particleEffectPlayer_->PlayParticle("Hit", { baseCharacter_->GetWorldPosition().x + downParticleData.particlePosition.x,
	//		baseCharacter_->GetWorldPosition().y + downParticleData.particlePosition.y, baseCharacter_->GetWorldPosition().z });
	//}

	if (downMoveData.isDownMove)
	{
		DownMove(downMoveData);
	}

	if (downAnimationTimer_ < 0 && baseCharacter_->GetBaseData().hp_ > 0)
	{
		EndDown();
	}
}

void CharacterDownState::DownMove(const DownMoveData downMoveData)
{
	//コンボがつながりやすくなるように移動する
	if (downAnimationTimer_ < downMoveData.moveTime)
	{
		Vector3 velocity = { 0.0f ,0.0f, 0.0f };
		velocity.x = (direction_ == Direction::Right) ? downMoveData.moveSpeed : -downMoveData.moveSpeed;
		baseCharacter_->Move(velocity);
	}
}

void CharacterDownState::HandleDown()
{
	std::string downType = baseCharacter_->GetDownType();

	if (downTable_.find(downType) != downTable_.end())
	{
		const DownData& data = downTable_[downType];

		// ダウン処理を実行
		Down(data.animationIndex, data.animationSpeed, data.particleData, data.moveData);
	}
}
