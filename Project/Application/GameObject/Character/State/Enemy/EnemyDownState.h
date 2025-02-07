/**
 * @file EnemyDownState.h
 * @brief 敵のダウン状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "EnemyBaseState.h"
#include "Application/GameObject/Character/State/CharacterDownState.h"
#include <unordered_map>

class EnemyDownState : public EnemyBaseState, CharacterDownState
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

	/// <summary>終了処理</summary>
	virtual void EndDown()override;
};

