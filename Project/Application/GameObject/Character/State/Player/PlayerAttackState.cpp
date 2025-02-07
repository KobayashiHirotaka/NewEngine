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
	//キャラクターをセット
	CharacterAttackState::SetCharacter(player_);

	//初期化
	CharacterAttackState::Initialize();
}

void PlayerAttackState::Update()
{
	//更新
	CharacterAttackState::Update();
}

void PlayerAttackState::EndAttack()
{
	player_->EndAttack(attackCancelData_.isCurrentAttack);
	player_->ResetCollision();
	player_->ChangeState(std::make_unique<PlayerRootState>());
}


