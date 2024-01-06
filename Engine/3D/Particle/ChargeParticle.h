#pragma once
#include "BaseParticle.h"
#include "Engine/3D/Particle/Random.h"

class ChargeParticle : public BaseParticle 
{
public:
	void Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime) override;

	void Update();

private:
	Vector3 center_{};
};

