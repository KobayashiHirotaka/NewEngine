#pragma once
#include "IScene.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Base/TextureManager/TextureManager.h"

#include "Project/GameObject/Character/Player/Player.h"
#include "Project/GameObject/Character/Enemy/Enemy.h"
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

	TextureManager* textureManager_ = nullptr;

	Input* input_ = nullptr;

	std::unique_ptr<Player>player_;
	std::unique_ptr<Model>playerModel_;

	std::unique_ptr<Enemy>enemy_;

	std::unique_ptr<Skydome>skydome_;
	std::unique_ptr<Model>skydomeModel_;

	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterPHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
};
