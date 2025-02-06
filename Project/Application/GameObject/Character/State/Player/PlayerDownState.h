/**
 * @file PlayerDownState.h
 * @brief プレイヤーのダウン状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"

class PlayerDownState : public PlayerBaseState
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

private:
};

