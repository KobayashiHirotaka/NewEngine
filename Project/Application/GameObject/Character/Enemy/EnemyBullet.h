/**
 * @file EnemyBullet.h
 * @brief 敵の弾の管理(移動、削除)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include "Engine/3D/Particle/ParticleEffectPlayer.h"

class EnemyBullet : public Collider
{
public:
	/// <summary>初期化</summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw(const Camera& camera);

	/// <summary>パーティクルの描画</summary>
	void DrawParticle(const Camera& camera);

	/// <summary>当たり判定</summary>
	void OnCollision(Collider* collider)override;

	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition() override;

	//Getter
	//WorldTransform
	WorldTransform& GetWorldTransform()override { return worldTransform_; }
	
	//IsDead
	bool GetIsDead() const { return isDead_; }

	//Setter
	//IsDead
	void SetIsDead(bool isDead) { isDead_ = isDead; }

private:
	//WorldTransform
	WorldTransform worldTransform_;

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

	//当たり判定
	AABB aabb_ = { {-0.2f,-0.2f,-0.2f},{0.2f,0.2f,0.2f} };
};
