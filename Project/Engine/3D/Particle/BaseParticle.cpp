/**
 * @file BaseParticle.cpp
 * @brief パーティクルの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "BaseParticle.h"

void BaseParticle::Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime)
{
	//位置を設定
	translation_ = translation;

	//回転を設定
	rotation_ = rotation;

	//スケールを設定
	scale_ = scale;

	//速度を設定
	velocity_ = velocity;

	//色を設定
	color_ = color;

	//寿命を設定
	lifeTime_ = lifeTime;

	//初期アルファ値を設定
	alpha_ = color_.w;
}

void BaseParticle::Update()
{
	//位置を速度に基づいて更新
	translation_ = Add(translation_, velocity_);

	//1フレームあたりの時間
	const float kDeltaTime = 1.0f / 60.0f;
	currentTime_ += kDeltaTime;

	//現在の時間に応じてアルファ値を変更
	color_.w = alpha_ - (currentTime_ / lifeTime_);

	//寿命が経過したらパーティクルを消す
	if (lifeTime_ < currentTime_)
	{
		isDead_ = true;
	}
}