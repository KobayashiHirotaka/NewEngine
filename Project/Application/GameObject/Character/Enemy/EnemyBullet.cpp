/**
 * @file EnemyBullet.cpp
 * @brief 敵の弾の管理(移動、削除)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EnemyBullet.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include <cassert>

void EnemyBullet::Initialize()
{

}

void EnemyBullet::Update()
{
	//更新
	BaseBullet::Update();
}

void EnemyBullet::Draw(const Camera& camera)
{
	//弾本体の描画
	BaseBullet::Draw(camera);
}

void EnemyBullet::DrawParticle(const Camera& camera)
{
	//弾のパーティクルの描画
	BaseBullet::DrawParticle(camera);
}

void EnemyBullet::ImGui()
{

}

void EnemyBullet::Create(Model* model, const Vector3& positon, const Vector3& velocity)
{
	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributeEnemyBullet);
	SetCollisionMask(kCollisionMaskEnemyBullet);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//弾の生成
	BaseBullet::Create(model, positon, velocity);
}

void EnemyBullet::OnCollision(Collider* collider)
{
	//敵と弾が当たった時の処理
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer || collider->GetCollisionAttribute() & kCollisionAttributePlayerBullet)
	{
		BaseBullet::SetIsDead(true);
	}
}

Vector3 EnemyBullet::GetWorldPosition()
{
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation.x;
	worldPos.y = worldTransform_.translation.y;
	worldPos.z = worldTransform_.translation.z;

	return worldPos;
}
