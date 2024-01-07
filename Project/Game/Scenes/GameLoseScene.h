#pragma once
#include "IScene.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Project/GameObject/Skydome/Skydome.h"
#include <memory>

class GameLoseScene : public IScene
{
public:
	GameLoseScene();

	~GameLoseScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;

private:
	WorldTransform worldTransform_;

	Camera camera_;

	TextureManager* textureManager_ = nullptr;

	Input* input_ = nullptr;

	std::unique_ptr<Skydome>skydome_;
	std::unique_ptr<Model>skydomeModel_;
};
