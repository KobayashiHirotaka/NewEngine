/**
 * @file PlayerRootState.h
 * @brief Playerの通常状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"

class PlayerRootState : public PlayerBaseState
{
public:
	enum MoveDirection
	{
		Flont,
		Back,
		Default,
		kCountOfMoveDirection,
	};

	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

private:
	/// <summary>移動</summary>
	void Move();

	/// <summary>ジャンプ</summary>
	void Jump();

	/// <summary>攻撃</summary>
	void Attack();

	/// <summary>移動時の押し出し処理</summary>
	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//移動の向き
	MoveDirection moveDirection_ = Flont;

	//速度
	float moveSpeed[MoveDirection::kCountOfMoveDirection] = { 0.04f, -0.04f, 0.0f };

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;
};

