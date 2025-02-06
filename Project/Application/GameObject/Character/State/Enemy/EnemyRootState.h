/**
 * @file EnemyRootState.h
 * @brief 敵の通常状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "EnemyBaseState.h"

class EnemyRootState : public EnemyBaseState
{
public:
	//移動の方向
	enum MoveDirection
	{
		Flont,
		Back,
		Default,
		kCountOfMoveDirection,
	};

	//アニメーション定数
	enum AnimationIndex
	{
		kAnimationBackMove = 0,
		kAnimationFlontMove = 1,
		kAnimationGuard = 2,
		kAnimationIdle = 5
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

	/// <summary>ダウン</summary>
	void Down();


	/// <summary>指定した範囲内のランダムな整数を生成</summary>
	int Random(int min_value, int max_value);

	/// <summary>ランダムに移動方向を選択</summary>
	int RandomMove();

	/// <summary>ランダムに攻撃または移動を選択</summary>
	int RandomAttackOrMove();

	/// <summary>ランダムに弾攻撃または移動を選択</summary>
	int RandomBulletOrMove();

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//移動の向き
	MoveDirection moveDirection_ = Flont;

	//速度
	float moveSpeed[MoveDirection::kCountOfMoveDirection] = { 0.04f, -0.04f, 0.0f };

	//再生するanimationの番号
	uint32_t animationIndex_ = 5;
	float animationTime_ = 0.0f;

	//行動のパターン
	int patternCount_ = 1;
	int moveTimer_ = 60;

	//ガード状態か
	bool isGuardMode_ = false;

	//ガードタイマー(確定反撃用)
	const int kGuardTime_ = 20;
	int guardTimer_ = kGuardTime_;

	//パターンカウントの定数
	const int kPatternCount_[8] = { 0,1,2,3,4,5,6,7 };

};

