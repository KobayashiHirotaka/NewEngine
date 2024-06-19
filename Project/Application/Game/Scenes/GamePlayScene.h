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
#include "Application/GameObject/Skydome.h"

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
	//textureManager
	TextureManager* textureManager_ = nullptr;

	//modelManager
	ModelManager* modelManager_ = nullptr;

	//input
	Input* input_ = nullptr;

	//audio
	Audio* audio_ = nullptr;

	//camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	std::unique_ptr<Player>player_;

	std::unique_ptr<Skydome> skydome_;
};
