#pragma once
#include "IScene.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/DebugCamera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/2D/Sprite/Sprite.h"
#include <memory>

class GameTitleScene : public IScene
{
public:
	GameTitleScene();

	~GameTitleScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

private:
	WorldTransform worldTransform_;
	WorldTransform groundWorldTransform_;

	Camera camera_;

	DebugCamera debugCamera_;

	TextureManager* textureManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> groundModel_;

	//サウンド
	uint32_t titleSoundHandle_ = 0u;

	std::unique_ptr<Light>light_;
	std::unique_ptr<PointLight>pointLight_;
	Vector3 lightDirection_;

	std::unique_ptr<Sprite>sprite_[2];
	int32_t textureHandle_[2] = {0u,0u};

	bool isDebugCamera_ = false;
};
