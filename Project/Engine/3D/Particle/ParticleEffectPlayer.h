#pragma once
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"

class ParticleEffectPlayer
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void PlayParticle(const char* name, Vector3 position);

private:
	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;
};

