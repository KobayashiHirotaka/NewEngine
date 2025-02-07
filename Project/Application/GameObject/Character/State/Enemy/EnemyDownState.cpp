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
	CharacterDownState::SetCharacter(enemy_);

	CharacterDownState::Initialize();
}

void EnemyDownState::Update()
{
	CharacterDownState::Update();
}

void EnemyDownState::EndDown()
{
	//アニメーションの設定
	const int kAnimationIdle = 5;
	enemy_->EndDownAnimation(kAnimationIdle);
	enemy_->ChangeState(std::make_unique<EnemyRootState>());
}
