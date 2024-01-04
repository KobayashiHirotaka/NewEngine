#include "Enemy.h"
#include <cassert>
#include <numbers>

void Enemy::Initialize(const std::vector<Model*>& models)
{
	input_ = Input::GetInstance();

	ICharacter::Initialize(models);
	worldTransform_.translation = { 7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 4.6f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,1.0f,0.0f };

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation.x = 0.5f;

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation.x = -0.5f;

	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Enemy::Update()
{
	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Enemy::Draw(const Camera& camera)
{
	models_[kModelIndexBody]->Draw(worldTransformBody_, camera);
	models_[kModelIndexHead]->Draw(worldTransformHead_, camera);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, camera);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, camera);
}

void Enemy::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		ImGui::Begin("Aaa");

		ImGui::End();
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributeWeapon)
	{
		ImGui::Begin("A");

		ImGui::End();
	}
}

Vector3 Enemy::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}



