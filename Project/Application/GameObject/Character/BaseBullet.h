/**
 * @file BaseBullet.h
 * @brief 各キャラクターの弾の基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/12/23
 */

#pragma once
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include "Application/GameObject/Effect/ParticleEffectPlayer.h"

class BaseBullet : public IGame3dObject
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw(const Camera& camera) = 0;

	/// <summary>パーティクルの描画</summary>
	virtual void DrawParticle(const Camera& camera) = 0;

	/// <summary>ImGui</summary>
	virtual void ImGui() = 0;

	/// <summary>生成</summary>
	virtual void Create(Model* model, const Vector3& position, const Vector3& velocity) = 0;

	//Getter
	//IsDead
	bool GetIsDead() const { return isDead_; }

	//Setter
	//IsDead
	void SetIsDead(bool isDead) { isDead_ = isDead; }

protected:
	//モデル
	Model* model_ = nullptr;

	//速度
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };

	//生きている時間
	static const int32_t kLifeTime = 100;

	//消えるまでの時間
	int32_t deathTimer_ = kLifeTime;

	//消えているかどうか
	bool isDead_ = false;

	//当たっているかどうか
	bool isHit_ = false;

	//パーティクル
	std::unique_ptr<ParticleEffectPlayer> particleEffectPlayer_;
};

