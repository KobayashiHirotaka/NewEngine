/**
 * @file Collider.h
 * @brief 当たり判定の基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/Utility/Math/MyMath.h"
#include "CollisionConfig.h"
#include <stdint.h>
#include <algorithm>

class Collider
{
private:
	//半径
	float radius_ = 1.0f;

	//AABB
	AABB aabb_ = { {-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f} };

	//属性
	uint32_t collisionAttribute_ = 0xffffffff;

	//マスク
	uint32_t collisionMask_ = 0xffffffff;

	//形状
	uint32_t collisionPrimitive_ = kCollisionPrimitiveSphere;

	//ダメージ
	float damage_ = 1.0f;

public:
	/// <summary>デストラクタ</summary>
	virtual ~Collider() {}

	/// <summary>当たり判定</summary>
	virtual void OnCollision(Collider* collider) = 0;

	//WorldPosition
	virtual Vector3 GetWorldPosition() = 0;

	//WorldTransform
	virtual WorldTransform& GetWorldTransform() = 0;

	//Radius
	float GetRadius() const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }

	//AABB
	AABB GetAABB() { return aabb_; };
	void SetAABB(AABB& aabb) { aabb_ = aabb; };

	//Attribute
	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }

	//Mask
	uint32_t GetCollisionMask() const { return collisionMask_; }
	void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }

	//Primitive
	uint32_t GetCollisionPrimitive() { return collisionPrimitive_; };
	void SetCollisionPrimitive(uint32_t collisionPrimitive) { collisionPrimitive_ = collisionPrimitive; };
};
