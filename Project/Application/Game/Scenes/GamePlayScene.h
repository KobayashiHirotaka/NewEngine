#pragma once
#include "IScene.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Utility/Collision/CollisionManager.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/2D/Sprite/Sprite.h"

#include <memory>

class GamePlayScene : public IScene
{
public:
	GamePlayScene();

	~GamePlayScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;

private:
	Camera camera_;

	TextureManager* textureManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;
};
