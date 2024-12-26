/**
 * @file Collider.h
 * @brief 当たり判定に必要なデータの管理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/Utility/Math/MyMath.h"
#include "CollisionConfig.h"
#include <stdint.h>
#include <algorithm>

 //前方宣言
class IGame3dObject;

class Collider
{
public:
	/// <summary>更新</summary>
	void Update();

	/// <summary>当たり判定</summary>
	void OnCollision(Collider* collider);

	//Getter,Setter
	//WorldPosition
	Vector3 GetWorldPosition() { return  worldPosition_; };

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

	//GameObject
	void SetGameObject(IGame3dObject* gameObject) { gameObject_ = gameObject; };

private:
	//ゲームオブジェクトのポインタ
	IGame3dObject* gameObject_ = nullptr;

	//ワールド座標
	Vector3 worldPosition_;

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
};
