#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"

class Player : public IGame3dObject
{
public:
	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

	void BoneDraw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	uint32_t GetAnimationIndex() { return animationIndex; };

private:
	//modelManager
	ModelManager* modelManager_ = nullptr;

	Input* input_ = nullptr;

	const Camera* camera_ = nullptr;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;

	bool isDebugCamera_ = false;

	uint32_t animationIndex = 0;

	std::unique_ptr<Light>light_;
	std::unique_ptr<PointLight>pointLight_;
	Vector3 lightDirection_;
};

