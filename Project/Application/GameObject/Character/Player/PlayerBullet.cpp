/**
 * @file PlayerBullet.cpp
 * @brief プレイヤーの弾の管理(移動、削除)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerBullet.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include <cassert>

void PlayerBullet::Initialize()
{
	
}

void PlayerBullet::Update()
{
	//更新
	BaseBullet::Update();
}

void PlayerBullet::Draw(const Camera& camera)
{
	//弾本体の描画
	BaseBullet::Draw(camera);
}

void PlayerBullet::DrawParticle(const Camera& camera)
{
	//弾のパーティクルの描画
	BaseBullet::DrawParticle(camera);
}

void PlayerBullet::ImGui()
{

}

void PlayerBullet::Create(Model* model, const Vector3& positon, const Vector3& velocity)
{
	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributePlayerBullet);
	SetCollisionMask(kCollisionMaskPlayerBullet);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//弾の生成
	BaseBullet::Create(model, positon, velocity);
}

void PlayerBullet::OnCollision(Collider* collider)
{
	//敵と弾が当たった時の処理
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy || collider->GetCollisionAttribute() & kCollisionAttributeEnemyBullet)
	{
		BaseBullet::SetIsDead(true);
	}
}

Vector3 PlayerBullet::GetWorldPosition()
{
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation.x;
	worldPos.y = worldTransform_.translation.y;
	worldPos.z = worldTransform_.translation.z;

	return worldPos;
}
