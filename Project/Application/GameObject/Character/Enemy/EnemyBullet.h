#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"

class EnemyBullet : public Collider
{
public:
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(const Camera& camera);

	void ParticleDraw(const Camera& camera);

	bool GetIsDead() const { return isDead_; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

	void OnCollision(Collider* collider, float damage)override;

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

private:
	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Vector3 velocity_;

	static const int32_t kLifeTime = 100;

	int32_t deathTimer_ = kLifeTime;

	bool isDead_ = false;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;

	//当たり判定
	AABB aabb_ = { {-0.2f,-0.2f,-0.2f},{0.2f,0.2f,0.2f} };

	bool isHit_ = false;
};
