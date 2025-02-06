/**
 * @file EnemyAttackState.h
 * @brief 敵の攻撃状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "EnemyBaseState.h"
#include <unordered_map>

class EnemyAttackState : public EnemyBaseState
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

	//攻撃時のパラメータ
	struct AttackData
	{
		int animationIndex;
		float animationSpeed;
		AttackMoveData moveData;
		AttackCancelData cancelData;
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

	/// <summary>攻撃を管理する</summary>
	void HandleAttack();

private:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//攻撃中の移動用パラメーター
	AttackMoveData attackMoveData_;

	//攻撃のキャンセル用パラメーター
	AttackCancelData attackCancelData_;

	//アニメーションフレーム
	int attackAnimationFrame_ = 0;

	//弾攻撃用のフラグ
	bool hasShot_ = false;

	//各攻撃のデータ
	std::unordered_map<std::string, AttackData> attackTable_;

	//攻撃ごとの定数
	//弱攻撃
	const int kLightPunchAnimationIndex_ = 12;
	const float kLightPunchAnimationSpeed_ = 1.5f;
	const AttackMoveData kLightPunchMoveData_ = { 0, 0.0f, false };

	//中攻撃(TC)
	const int kTCMiddlePunchAnimationIndex_ = 11;
	const float kTCMiddlePunchAnimationSpeed_ = 1.5f;
	const AttackMoveData kTCMiddlePunchMoveData_ = { 5, 0.03f, true };

	//強攻撃
	const int kHighPunchAnimationIndex_ = 3;
	const float kHighPunchAnimationSpeed_ = 1.8f;
	const AttackMoveData kHighPunchMoveData_ = { 15, 0.03f, true };

	//弾攻撃
	const int kShotAnimationIndex_ = 1;
	const float kShotAnimationSpeed_ = 1.2f;
	const AttackMoveData kShotMoveData_ = { 0, 0.0f, false };
};

