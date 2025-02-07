/**
 * @file PlayerAttackState.h
 * @brief プレイヤーの攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"
#include "Application/GameObject/Character/State/CharacterAttackState.h"
#include <unordered_map>

class PlayerAttackState : public PlayerBaseState, CharacterAttackState
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

	/// <summary>終了処理</summary>
	virtual void EndAttack()override;
};

