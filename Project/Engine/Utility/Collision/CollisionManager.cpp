/**
 * @file CollisionManager.cpp
 * @brief 当たり判定の管理(球やAABBなど)
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "CollisionManager.h"

void CollisionManager::CheckAllCollision()
{
	//コライダーリストを順番に比較して衝突をチェック
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{

		std::list<Collider*>::iterator itrB = itrA;
		++itrB;
		for (; itrB != colliders_.end(); ++itrB)
		{
			//2つのコライダー間の衝突をチェック
			CheckCollisionPair(*(itrA), *(itrB));
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	//衝突属性とマスクが一致しない場合は衝突しない
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
		(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0)
	{
		return;
	}

	//球と球の判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0) ||
		((colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0))
	{
		Vector3 posA = colliderA->GetWorldPosition();
		Vector3 posB = colliderB->GetWorldPosition();

		float distance = Length(Subtract(posA, posB));

		//距離が半径の合計以下なら衝突
		if (distance <= colliderA->GetRadius() + colliderB->GetRadius())
		{
			colliderA->OnCollision(colliderB);
			colliderB->OnCollision(colliderA);
		}
	}

	//AABBとAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0))
	{

		Vector3 posA = colliderA->GetWorldPosition();
		Vector3 posB = colliderB->GetWorldPosition();

		AABB aabbA = colliderA->GetAABB();
		AABB aabbB = colliderB->GetAABB();

		//AABB同士が重なっていたら衝突
		if (posA.x + aabbA.min.x <= posB.x + aabbB.max.x && posA.x + aabbA.max.x >= posB.x + aabbB.min.x &&
			posA.y + aabbA.min.y <= posB.y + aabbB.max.y && posA.y + aabbA.max.y >= posB.y + aabbB.min.y &&
			posA.z + aabbA.min.z <= posB.z + aabbB.max.z && posA.z + aabbA.max.z >= posB.z + aabbB.min.z)
		{
			colliderA->OnCollision(colliderB);
			colliderB->OnCollision(colliderA);
		}
	}

	//球とAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0))
	{
		Vector3 posA = colliderA->GetWorldPosition();
		Vector3 posB = colliderB->GetWorldPosition();

		AABB aabbA = colliderA->GetAABB();
		AABB aabbB = colliderB->GetAABB();

		//球の最寄りの点とAABBとの距離を比較
		if (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere)
		{
			Vector3 closestPoint
			{
				std::clamp(posA.x,posB.x + aabbB.min.x,posB.x + aabbB.max.x),
				std::clamp(posA.y,posB.y + aabbB.min.y,posB.y + aabbB.max.y),
				std::clamp(posA.z,posB.z + aabbB.min.z,posB.z + aabbB.max.z)
			};

			float distance = Length(Subtract(closestPoint, posA));

			//距離が半径以下なら衝突
			if (distance <= colliderA->GetRadius())
			{
				colliderA->OnCollision(colliderB);
				colliderB->OnCollision(colliderA);
			}

		}
		else if (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) {

			Vector3 closestPoint
			{
				std::clamp(posB.x,posA.x + aabbA.min.x,posA.x + aabbA.max.x),
				std::clamp(posB.y,posA.y + aabbA.min.y,posA.y + aabbA.max.y),
				std::clamp(posB.z,posA.z + aabbA.min.z,posA.z + aabbA.max.z)
			};

			float distance = Length(Subtract(closestPoint, posB));

			//距離が半径以下なら衝突
			if (distance <= colliderB->GetRadius())
			{
				colliderA->OnCollision(colliderB);
				colliderB->OnCollision(colliderA);
			}
		}
	}
}