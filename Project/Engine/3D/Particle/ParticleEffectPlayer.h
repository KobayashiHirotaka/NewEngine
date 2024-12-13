/**
 * @file ParticleEmitter.h
 * @brief パーティクルエフェクトの生成、再生などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"

class ParticleEffectPlayer
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw(const Camera& camera);

	/// <summary>パーティクルeffectを再生</summary>
	void PlayParticle(const char* name, Vector3 position);

private:
	//モデル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;

	//パーティクルシステム
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;
};

