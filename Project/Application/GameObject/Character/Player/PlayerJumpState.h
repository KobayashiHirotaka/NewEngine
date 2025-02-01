/**
 * @file PlayerJumpState.h
 * @brief Playerのジャンプ状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/IState.h"

class PlayerJumpState : public IState
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

private:

};

