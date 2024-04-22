#pragma once
#include <list>
#include"Collider.h"

class CollisionManager
{
public:
	void AddCollider(Collider* collider) { colliders_.push_back(collider); }
	void ClearColliders() { colliders_.clear(); }
	void CheckAllCollision();
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
private:
	std::list<Collider*> colliders_;
};
