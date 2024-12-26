/**
 * @file BaseBullet.cpp
 * @brief 各キャラクターの弾の基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 12/23
 */

#include "BaseBullet.h"

void BaseBullet::Initialize()
{

}

void BaseBullet::Update()
{
	//弾を移動させる処理
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	//弾を消す処理
	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}

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

	//パーティクルエフェクトプレイヤーの更新
	particleEffectPlayer_->Update();

	//Lightingの設定
	model_->GetLight()->SetEnableLighting(false);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void BaseBullet::Draw(const Camera& camera)
{
	//弾本体の描画
	if (isDead_ == false)
	{
		model_->Draw(worldTransform_, camera, 0);
	}
}

void BaseBullet::DrawParticle(const Camera& camera)
{
	//弾のパーティクルの描画
	if (isDead_ == false)
	{
		particleEffectPlayer_->Draw(camera);
	}
}

void BaseBullet::Create(Model* model, const Vector3& positon, const Vector3& velocity)
{
	assert(model);

	//モデルの設定
	model_ = model;

	//worldTransformの初期化
	worldTransform_.Initialize();
	worldTransform_.translation = positon;

	//速度の設定
	velocity_ = velocity;

	//パーティクルエフェクトプレイヤーの生成
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}
