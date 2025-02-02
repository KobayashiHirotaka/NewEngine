/**
 * @file PlayerRootState.h
 * @brief Playerの通常状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"

class PlayerRootState : public PlayerBaseState
{
public:
	enum MoveDirection
	{
		Flont,
		Back,
		Default
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

	/// <summary>移動時の押し出し処理</summary>
	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	MoveDirection moveDirection_ = Flont;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;
};

