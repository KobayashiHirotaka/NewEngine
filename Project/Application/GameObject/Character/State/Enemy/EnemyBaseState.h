/**
 * @file EnemyBaseState.h
 * @brief 敵の状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"

class Enemy;

class EnemyBaseState
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~EnemyBaseState() = default;

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	//Setter
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

protected:
	//Enemyのポインタ
	Enemy* enemy_;
};
