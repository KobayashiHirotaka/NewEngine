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
	CharacterDownState::SetCharacter(player_);

	CharacterDownState::Initialize();
}

void PlayerDownState::Update()
{
	CharacterDownState::Update();
}

void PlayerDownState::EndDown()
{
	//アニメーションの設定
	const int kAnimationIdle = 5;
	player_->EndDownAnimation(kAnimationIdle);
	player_->ChangeState(std::make_unique<PlayerRootState>());
}
