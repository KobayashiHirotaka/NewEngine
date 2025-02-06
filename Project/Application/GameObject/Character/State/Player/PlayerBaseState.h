/**
 * @file PlayerBaseState.h
 * @brief プレイヤーの状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"

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
};
