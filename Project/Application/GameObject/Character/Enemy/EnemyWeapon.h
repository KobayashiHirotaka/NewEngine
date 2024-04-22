#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include <numbers>

class EnemyWeapon : public Collider
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	WorldTransform& GetWorldTransform() override { return worldTransform_; };

	Vector3 GetWorldPosition() override;

	void OnCollision(Collider* collider, float damage) override;

	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; };

	void Attack();

	bool GetIsAttack() { return isAttack_; };

	void SetIsAttack(bool isAttack) { isAttack_ = isAttack; };

	void SetTranslation(const Vector3& translation) { worldTransform_.translation = translation; };

	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; }

private:
	void AttackInitialize();

	std::unique_ptr<Model> enemyWeaponModel_;

	WorldTransform worldTransform_;
	WorldTransform collisionWorldTransform_;

	bool isHit_ = false;

	bool isAttack_ = false;
};

