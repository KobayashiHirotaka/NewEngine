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

	//パーティクルの再生
	if (velocity_.x < 0.0f)
	{
		particleEffectPlayer_->PlayParticle("EnemyLeftBullet", { worldTransform_.translation.x,
					worldTransform_.translation.y,worldTransform_.translation.z });
	}
	else
	{
		particleEffectPlayer_->PlayParticle("EnemyRightBullet", { worldTransform_.translation.x,
					worldTransform_.translation.y,worldTransform_.translation.z });
	}

	//当たり判定の更新
	collider_->Update();
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
	collider_ = std::make_unique<Collider>();
	collider_->SetAABB(aabb_);

	collider_->SetCollisionAttribute(kCollisionAttributeEnemyBullet);
	collider_->SetCollisionMask(kCollisionMaskEnemyBullet);
	collider_->SetCollisionPrimitive(kCollisionPrimitiveAABB);

	collider_->SetGameObject(this);

	//弾の生成
	BaseBullet::Create(model, positon, velocity);
}

void EnemyBullet::OnCollision(Collider* collider)
{
	//プレイヤーと弾が当たった時の処理
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer || collider->GetCollisionAttribute() & kCollisionAttributePlayerBullet)
	{
		isDead_ = true;
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
