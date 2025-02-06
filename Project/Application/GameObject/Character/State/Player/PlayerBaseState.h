/**
 * @file PlayerBaseState.h
 * @brief プレイヤーの状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Application/GameObject/Character/Direction.h"

class Player;

class PlayerBaseState
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~PlayerBaseState() = default;

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	//Setter
	void SetPlayer(Player* player) { player_ = player; };

protected:
	//Playerのポインタ
	Player* player_;

	//アニメーション用のパラメーター
	uint32_t animationIndex_ = 5;
	float animationTime_ = 0.0f;
	const float kScaleFacter_ = 100.0f;

	//キャラクターの向き
	Direction direction_ = Direction::Right;
};
