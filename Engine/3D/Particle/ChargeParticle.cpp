#include "ChargeParticle.h"

void ChargeParticle::Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime)
{
	center_ = translation;

	rotation_ = rotation;

	scale_ = scale;

	velocity_ = velocity;

	color_ = color;

	lifeTime_ = lifeTime;
	
	alpha_ = color_.w;
	
	translation_ = { center_.x + Random::GetRandomFloat(-2.0f,2.0f),center_.y + Random::GetRandomFloat(-2.0f,2.0f),0.0f };
}

void ChargeParticle::Update()
{
	const float speed = 0.1f;
	float deltaX = center_.x - translation_.x;
	float deltaY = center_.y - translation_.y;
	float length = sqrt(deltaX * deltaX + deltaY * deltaY);

	if (length > 0)
	{
		deltaX /= length;
		deltaY /= length;
		translation_.x += deltaX * speed;
		translation_.y += deltaY * speed;
	}

	const float kDeltaTime = 1.0f / 60.0f;
	currentTime_ += kDeltaTime;
	color_.w = alpha_ - (currentTime_ / lifeTime_);

	if (currentTime_ > lifeTime_)
	{
		isDead_ = true;
	}
}