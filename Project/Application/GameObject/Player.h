#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"

class Player
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

private:
	Input* input_ = nullptr;

	const Camera* camera_ = nullptr;

	WorldTransform worldTransform_;

	std::unique_ptr<Model> modelFighterBody_;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;

	bool isDebugCamera_ = false;
};

