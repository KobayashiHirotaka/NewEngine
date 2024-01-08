#pragma once
#include "Engine/Utility/Math/MyMath.h"

class BaseParticle
{
public:
	virtual void Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime);

	virtual void Update();

	virtual const Vector3& GetTranslation() { return translation_; };

	virtual const Vector3& GetRotation() { return rotation_; };

	virtual const Vector3& GetScale() { return scale_; };

	virtual const Vector4& GetColor() { return color_; };

	virtual const bool IsDead() { return isDead_; };

protected:
	Vector3 translation_;

	Vector3 rotation_;
	
	Vector3 scale_;

	Vector3 velocity_;

	Vector4 color_;

	float lifeTime_ = 0.0f;

	float currentTime_ = 0.0f;

	bool isDead_ = false;
	
	float alpha_ = 0.0f;
};