/**
 * @file PlayerAttackState.h
 * @brief Playerの攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"

class PlayerAttackState : public PlayerBaseState
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;

	int attackAnimationFrame_ = 0;
	const float kScaleFacter_ = 100.0f;
};

