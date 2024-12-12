/**
 * @file CollisionManager.h
 * @brief 当たり判定の管理(球やAABBなど)
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <list>
#include"Collider.h"

class CollisionManager
{
public:
	/// <summary>コライダーを追加</summary>
	void AddCollider(Collider* collider) { colliders_.push_back(collider); }

	/// <summary>コライダーをクリア</summary>
	void ClearColliders() { colliders_.clear(); }

	/// <summary>すべてのコライダー同士の衝突をチェック</summary>
	void CheckAllCollision();

	/// <summary>2つのコライダー間の衝突をチェック</summary>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private:
	//コライダーのリスト
	std::list<Collider*> colliders_;
};
