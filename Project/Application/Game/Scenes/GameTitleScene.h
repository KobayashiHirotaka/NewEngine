#pragma once
#include "IScene.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite/Sprite.h"
#include <memory>

class GameTitleScene : public IScene
{
public:
	GameTitleScene();

	~GameTitleScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;

private:
	WorldTransform worldTransform_;
	WorldTransform groundWorldTransform_;

	Camera camera_;

	TextureManager* textureManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> groundModel_;

	//サウンド
	uint32_t titleSoundHandle_ = 0u;

	std::unique_ptr<Light>light_;
	std::unique_ptr<PointLight>pointLight_;
	Vector3 lightDirection_;
};
