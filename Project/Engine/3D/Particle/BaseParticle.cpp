#include "BaseParticle.h"

void BaseParticle::Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime)
{
	translation_ = translation;

	rotation_ = rotation;

	scale_ = scale;

	velocity_ = velocity;

	color_ = color;

	lifeTime_ = lifeTime;

	alpha_ = color_.w;
}

void BaseParticle::Update()
{
	translation_ = Add(translation_, velocity_);

	const float kDeltaTime = 1.0f / 60.0f;
	currentTime_ += kDeltaTime;
	color_.w = alpha_ - (currentTime_ / lifeTime_);

	if (lifeTime_ < currentTime_)
	{
		isDead_ = true;
	}
}