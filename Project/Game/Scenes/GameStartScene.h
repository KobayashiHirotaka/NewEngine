#pragma once
#include "IScene.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Project/GameObject/Player/Player.h"
#include "Project/GameObject/Skydome/Skydome.h"
#include <memory>

class GameStartScene : public IScene
{
public:
	GameStartScene();

	~GameStartScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;

private:
	WorldTransform worldTransform_;

	Camera camera_;

	Input* input_ = nullptr;

	std::unique_ptr<Player>player_;
	std::unique_ptr<Model>playerModel_;

	std::unique_ptr<Skydome>skydome_;
	std::unique_ptr<Model>skydomeModel_;
};
