#pragma once
#include "IScene.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Project/GameObject/Skydome/Skydome.h"
#include <memory>

class GameWinScene : public IScene
{
public:
	GameWinScene();

	~GameWinScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;

private:
	WorldTransform worldTransform_;

	Camera camera_;

	TextureManager* textureManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	std::unique_ptr<Skydome>skydome_;
	std::unique_ptr<Model>skydomeModel_;

	std::unique_ptr<Sprite>winSceneSprite_ = nullptr;
	uint32_t winSceneTextureHandle_ = 0;

	//サウンド
	uint32_t selectSoundHandle_ = 0u;
};

