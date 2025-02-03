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
		//アニメーション
		const int kAnimationLighhtPunch = 13;
		const float animationSpeed = 1.5f;

		animationIndex_ = kAnimationLighhtPunch;

		if (!player_->GetIsDown())
		{
			player_->UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, player_->GetModel());
		}

		player_->SetAnimationIndex(animationIndex_);

		if (attackAnimationFrame_ > player_->GetAttackData().recoveryTime)
		{
			//player_->EndAttack(player_->GetAttackData().isLightPunch);
			player_->ResetCollision();
			player_->ChangeState(std::make_unique<PlayerRootState>());
		}

		attackAnimationFrame_ += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}
}
