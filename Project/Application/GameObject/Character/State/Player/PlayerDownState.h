/**
 * @file PlayerDownState.h
 * @brief プレイヤーのダウン状態を管理するクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "PlayerBaseState.h"
#include "Application/GameObject/Effect/ParticleEffectPlayer.h"
#include <unordered_map>

class PlayerDownState : public PlayerBaseState
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
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

private:
	/// <summary>ダウン</summary>
	void Down(const int kAnimationIndex, const float animationSpeed, const DownParticleData downParticleData,
		const DownMoveData downMoveData);

	/// <summary>ダウン中の移動</summary>
	void DownMove(const DownMoveData downMoveData);

	/// <summary>ダウンを管理する</summary>
	void HandleDown();

private:
	//ダウン中の移動用のパラメーター
	DownMoveData downMoveData_;

	//ダウン中のパーティクル用のパラメーター
	DownParticleData downParticleData_;

	//ダウンアニメーションタイマー
	int downAnimationTimer_ = 60;

	//各ダウンのデータ
	std::unordered_map<std::string, DownData> downTable_;

	//攻撃ごとの定数
	//軽ダウン
	const int kAnimationLightDown = 4;
	const float lightDownAnimationSpeed_ = 1.5f;
	const DownMoveData kLightMoveData_ = { 0, 0.0f, false };
	const DownParticleData kLightParticleData_ = { 55, 0.1f, {(direction_ == Direction::Right) ? -0.1f : 0.1f, 0.5f} };

	//パーティクル
	std::unique_ptr<ParticleEffectPlayer> particleEffectPlayer_;
};

