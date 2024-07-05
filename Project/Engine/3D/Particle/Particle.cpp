#include "Particle.h"

void Particle::Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime)
{
	translation_ = translation;

	rotation_ = rotation;

	scale_ = { scale.x,scale.x,scale.x };

	velocity_ = velocity;

	color_ = color;

	lifeTime_ = lifeTime;
}

void Particle::Update()
{
	float scale = scale_.x;
	scale -= 0.01f;

	if (scale < 0.0f)
	{
		scale = 0.0f;
		isDead_ = true;
	}

	scale_ = { scale,scale,scale };
	translation_ = Add(translation_, velocity_);
}