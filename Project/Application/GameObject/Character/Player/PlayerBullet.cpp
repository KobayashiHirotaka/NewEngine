/**
 * @file PlayerBullet.cpp
 * @brief プレイヤーの弾の管理(移動、削除)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PlayerBullet.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& positon, const Vector3& velocity)
{
	assert(model);

	//モデルの設定
	model_ = model;

	//worldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.translation = positon;

	//速度の設定
	velocity_ = velocity;

	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributePlayerBullet);
	SetCollisionMask(kCollisionMaskPlayerBullet);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//パーティクルエフェクトプレイヤーの生成
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void PlayerBullet::Update()
{
	//弾を移動させる処理
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	//弾を消す処理
	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}

	//パーティクルエフェクトプレイヤーの更新
	particleEffectPlayer_->Update();

	//パーティクルの再生
	if (velocity_.x < 0.0f)
	{
		particleEffectPlayer_->PlayParticle("PlayerLeftBullet", { worldTransform_.translation.x,
					worldTransform_.translation.y,worldTransform_.translation.z });
	}
	else
	{
		particleEffectPlayer_->PlayParticle("PlayerRightBullet", { worldTransform_.translation.x,
					worldTransform_.translation.y,worldTransform_.translation.z });
	}

	//Lightingの設定
	model_->GetLight()->SetEnableLighting(false);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void PlayerBullet::Draw(const Camera& camera)
{
	//弾本体の描画
	if (isDead_ == false)
	{
		model_->Draw(worldTransform_, camera, 0);
	}
}

void PlayerBullet::DrawParticle(const Camera& camera)
{
	//弾のパーティクルの描画
	if (isDead_ == false)
	{
		particleEffectPlayer_->Draw(camera);
	}
}


void PlayerBullet::OnCollision(Collider* collider)
{
	//敵と弾が当たった時の処理
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy || collider->GetCollisionAttribute() & kCollisionAttributeEnemyBullet)
	{
		isDead_ = true;
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
