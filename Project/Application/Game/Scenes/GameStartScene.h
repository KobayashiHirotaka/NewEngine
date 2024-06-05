#pragma once
#include "IScene.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Application/GameObject/Skydome/Skydome.h"
#include "Engine/Utility/Level/LevelData.h"
#include <memory>

class GameStartScene : public IScene
{
public:
	static const int kTransitionTime = 60;

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

	LevelData* levelData_ = nullptr;

	std::unique_ptr<Skydome>skydome_;
	std::unique_ptr<Model>skydomeModel_;

	std::unique_ptr<Sprite>titleSprite_ = nullptr;
	uint32_t titleTextureHandle_ = 0;
	Vector2 titleSpritePosition_ = { 0.0f,-25.0f };
	float titleSpriteMoveSpeed_ = 1.5f;
	int titleSpriteMoveTimer_ = 30;

	std::unique_ptr<Sprite>titleUISprite_ = nullptr;
	uint32_t titleUITextureHandle_ = 0;

	std::unique_ptr<Sprite>generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	std::unique_ptr<Sprite>attackCommandListSprite_ = nullptr;
	uint32_t attackCommandListTextureHandle_ = 0;

	//サウンド
	uint32_t titleSoundHandle_ = 0u;
	uint32_t selectSoundHandle_ = 0u;

	int spriteCount_ = 0;
	bool isOpen_ = false;

	//トランジション
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	//モデル
	std::map < std::string,  Model*> models_;
};
