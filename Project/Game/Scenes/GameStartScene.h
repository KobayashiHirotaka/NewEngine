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

	Audio* audio_ = nullptr;

	std::unique_ptr<Skydome>skydome_;
	std::unique_ptr<Model>skydomeModel_;

	std::unique_ptr<Sprite>titleSprite_ = nullptr;
	uint32_t titleTextureHandle_ = 0;

	std::unique_ptr<Sprite>commandListSprite_ = nullptr;
	uint32_t commandListTextureHandle_ = 0;

	//サウンド
	uint32_t soundHandle_ = 0u;
	uint32_t titleSoundHandle_ = 0u;

	bool isOpen_ = false;
};
