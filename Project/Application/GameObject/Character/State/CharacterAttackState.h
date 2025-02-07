/**
 * @file CharacterAttackState.h
 * @brief 各キャラクターの攻撃状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Application/GameObject/Character/Direction.h"
#include <unordered_map>

class BaseCharacter;

class CharacterAttackState
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
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	//Setter
	void SetCharacter(BaseCharacter* baseCharacter) { baseCharacter_ = baseCharacter; };

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

protected:
	/// <summary>終了処理</summary>
	virtual void EndAttack() = 0;

protected:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//攻撃中の移動用パラメーター
	AttackMoveData attackMoveData_;

	//攻撃のキャンセル用パラメーター
	AttackCancelData attackCancelData_;

	//キャラクター
	BaseCharacter* baseCharacter_;

	//アニメーション用のパラメーター
	uint32_t animationIndex_ = 5;
	float animationTime_ = 0.0f;
	const float kScaleFacter_ = 100.0f;

	//キャラクターの向き
	Direction direction_ = Direction::Right;

	//アニメーションフレーム
	int attackAnimationFrame_ = 0;

	//弾攻撃用のフラグ
	bool hasShot_ = false;

	//各攻撃のデータ
	std::unordered_map<std::string, AttackData> attackTable_;

	//攻撃ごとの定数
	//弱攻撃
	const int kLightPunchAnimationIndex_ = 13;
	const float kLightPunchAnimationSpeed_ = 1.5f;
	const AttackMoveData kLightPunchMoveData_ = { 0, 0.0f, false };

	//中攻撃(TC)
	const int kTCMiddlePunchAnimationIndex_ = 12;
	const float kTCMiddlePunchAnimationSpeed_ = 1.5f;
	const AttackMoveData kTCMiddlePunchMoveData_ = { 5, 0.03f, true };

	//強攻撃
	const int kHighPunchAnimationIndex_ = 3;
	const float kHighPunchAnimationSpeed_ = 1.8f;
	const AttackMoveData kHighPunchMoveData_ = { 15, 0.03f, true };

	//弾攻撃
	const int kShotAnimationIndex_ = 19;
	const float kShotAnimationSpeed_ = 1.2f;
	const AttackMoveData kShotMoveData_ = { 0, 0.0f, false };
};

