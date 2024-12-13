/**
 * @file BaseParticle.h
 * @brief パーティクルの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Utility/Math/MyMath.h"

class BaseParticle
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime);

	/// <summary>更新</summary>
	virtual void Update();

	//Getter
	//Translation
	virtual const Vector3& GetTranslation() { return translation_; };

	//Rotation
	virtual const Vector3& GetRotation() { return rotation_; };

	//Scale
	virtual const Vector3& GetScale() { return scale_; };

	//Color
	virtual const Vector4& GetColor() { return color_; };

	//IsDead
	virtual const bool IsDead() { return isDead_; };

protected:
	//位置
	Vector3 translation_;

	//回転
	Vector3 rotation_;

	//スケール
	Vector3 scale_;

	//速度
	Vector3 velocity_;

	//色
	Vector4 color_;

	//寿命
	float lifeTime_ = 0.0f;

	//現在の時間
	float currentTime_ = 0.0f;

	//生きているか
	bool isDead_ = false;

	//アルファ値
	float alpha_ = 0.0f;
};