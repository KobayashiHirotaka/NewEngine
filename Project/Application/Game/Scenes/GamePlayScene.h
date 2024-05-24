#pragma once
#include "IScene.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/DebugCamera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Utility/Collision/CollisionManager.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/2D/Sprite/Sprite.h"
#include <memory>

#include "Application/GameObject/Player.h"

class GamePlayScene : public IScene
{
public:
	GamePlayScene();

	~GamePlayScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

private:
	Camera camera_;

	TextureManager* textureManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	std::unique_ptr<Light>light_;
	std::unique_ptr<PointLight>pointLight_;
	Vector3 lightDirection_;

	std::unique_ptr<Player>player_;

	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;
};
