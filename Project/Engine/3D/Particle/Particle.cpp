/**
 * @file Particle.cpp
 * @brief パーティクルの初期化、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Particle.h"

void Particle::Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime)
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
}

void Particle::Update()
{
	//現在のスケールのXを取得
	float scale = scale_.x;

	//スケールが変化する速さ
	const float kScaleSpeed = 0.01f;

	//スケールを徐々に小さくする
	scale -= kScaleSpeed;

	//スケールが0になったら、パーティクルはを消す
	if (scale < 0.0f)
	{
		scale = 0.0f;
		isDead_ = true;
	}

	//新しいスケールの適用
	scale_ = { scale, scale, scale };

	//位置の加算
	translation_ = Add(translation_, velocity_);
}