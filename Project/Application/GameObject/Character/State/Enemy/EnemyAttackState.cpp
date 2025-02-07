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
	//キャラクターをセット
	CharacterAttackState::SetCharacter(enemy_);

	//初期化
	CharacterAttackState::Initialize();
}

void EnemyAttackState::Update()
{
	//更新
	CharacterAttackState::Update();
}

void EnemyAttackState::EndAttack()
{
	enemy_->EndAttack(attackCancelData_.isCurrentAttack);
	enemy_->ResetCollision();
	enemy_->ChangeState(std::make_unique<EnemyRootState>());
}
