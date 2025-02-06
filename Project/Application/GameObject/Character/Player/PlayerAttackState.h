/**
 * @file PlayerAttackState.h
 * @brief プレイヤーの攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"
#include "Application/GameObject/Character/Direction.h"

class PlayerAttackState : public PlayerBaseState
{
public:
	//攻撃の種類
	enum class AttackType
	{
		LightPunch,
		TargetMiddlePunch
	};

	//攻撃中の移動用のパラメータ
	struct AttackMoveData
	{
		int moveTime;
		float moveSpeed;
		bool isAttackMove;
	};

	//攻撃のキャンセル用のパラメータ
	struct AttackCancelData
	{
		std::string attackType;
		bool isCurrentAttack;
		bool isNextAttack;
		bool isAttackCancel;
	};

	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

private:
	/// <summary>攻撃</summary>
	void Attack(const int kAnimationIndex, const float animationSpeed, const AttackMoveData attackMoveData,
		const AttackCancelData attackCancelData);

	/// <summary>弾攻撃</summary>
	void Shot(const Vector3 position);

	/// <summary>攻撃中の移動</summary>
	void AttackMove(const AttackMoveData attackMoveData);

	/// <summary>キャンセル</summary>
	void Cancel(AttackCancelData attackCancelData);

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//アニメーション用のパラメーター
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;
	int attackAnimationFrame_ = 0;
	const float kScaleFacter_ = 100.0f;

	//攻撃中の移動用パラメーター
	AttackMoveData attackMoveData_;

	//攻撃のキャンセル用パラメーター
	AttackCancelData attackCancelData_;

	//キャラクターの向き
	Direction direction_ = Direction::Right;

	//弾攻撃用のフラグ
	bool hasShot_ = false;
};

