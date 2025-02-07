/**
 * @file CharacterDownState.h
 * @brief 各キャラクターのダウン状態を管理する基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Application/GameObject/Character/Direction.h"
#include <unordered_map>

class BaseCharacter;

class CharacterDownState
{
public:
	//ダウン中の移動用のパラメーター
	struct DownMoveData
	{
		int moveTime;
		float moveSpeed;
		bool isDownMove;
	};

	//ダウン中のパーティクル用のパラメーター
	struct DownParticleData
	{
		int particleTime;
		float particleMoveSpeed;
		Vector2 particlePosition;
	};

	//ダウン時のパラメーター
	struct DownData
	{
		int animationIndex;
		float animationSpeed;
		DownMoveData moveData;
		DownParticleData particleData;
	};

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;


	//Setter
	void SetCharacter(BaseCharacter* baseCharacter) { baseCharacter_ = baseCharacter; };

private:
	/// <summary>ダウン</summary>
	void Down(const int kAnimationIndex, const float animationSpeed, const DownParticleData downParticleData,
		const DownMoveData downMoveData);

	/// <summary>ダウン中の移動</summary>
	void DownMove(const DownMoveData downMoveData);

	/// <summary>ダウンを管理する</summary>
	void HandleDown();

protected:
	/// <summary>終了処理</summary>
	virtual void EndDown() = 0;

protected:
	//ダウン中の移動用のパラメーター
	DownMoveData downMoveData_;

	//ダウン中のパーティクル用のパラメーター
	DownParticleData downParticleData_;

	//キャラクター
	BaseCharacter* baseCharacter_;

	//ダウンアニメーションタイマー
	int downAnimationTimer_ = 60;

	//各ダウンのデータ
	std::unordered_map<std::string, DownData> downTable_;

	//アニメーション用のパラメーター
	uint32_t animationIndex_ = 5;
	float animationTime_ = 0.0f;
	const float kScaleFacter_ = 100.0f;

	//キャラクターの向き
	Direction direction_ = Direction::Right;

	//攻撃ごとの定数
	//軽ダウン
	const int kAnimationLightDown = 4;
	const float lightDownAnimationSpeed_ = 1.5f;
	const DownMoveData kLightMoveData_ = { 0, 0.0f, false };
	const DownParticleData kLightParticleData_ = { 55, 0.1f, {(direction_ == Direction::Right) ? -0.1f : 0.1f, 0.5f} };
};

