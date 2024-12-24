/**
 * @file Collider.cpp
 * @brief 当たり判定のクラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Collider.h"

void Collider::Update()
{
	worldPosition_.x = gameObject_->GetWorldTransform().matWorld.m[3][0];
	worldPosition_.y = gameObject_->GetWorldTransform().matWorld.m[3][1];
	worldPosition_.z = gameObject_->GetWorldTransform().matWorld.m[3][2];
}

void Collider::OnCollision(Collider* collider)
{
	gameObject_->OnCollision(collider);
}
