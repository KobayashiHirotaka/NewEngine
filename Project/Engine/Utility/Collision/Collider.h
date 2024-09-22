#pragma once
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/Utility/Math/MyMath.h"
#include "CollisionConfig.h"
#include <stdint.h>
#include <algorithm>

class Collider
{
private:
	float radius_ = 1.0f;
	AABB aabb_ = { {-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f} };
	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t collisionMask_ = 0xffffffff;
	uint32_t collisionPrimitive_ = kCollisionPrimitiveSphere;
	float damage_ = 1.0f;

public:
	float GetRadius() const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }
	AABB GetAABB() { return aabb_; };
	void SetAABB(AABB& aabb) { aabb_ = aabb; };
	virtual ~Collider() {}
	virtual void OnCollision(Collider* collider) = 0;
	virtual Vector3 GetWorldPosition() = 0;
	virtual WorldTransform& GetWorldTransform() = 0;
	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }
	uint32_t GetCollisionMask() const { return collisionMask_; }
	void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }
	uint32_t GetCollisionPrimitive() { return collisionPrimitive_; };
	void SetCollisionPrimitive(uint32_t collisionPrimitive) { collisionPrimitive_ = collisionPrimitive; };
	float GetDamage() { return damage_; };
	void SetDamage(float damage) { damage_ = damage; };
};
