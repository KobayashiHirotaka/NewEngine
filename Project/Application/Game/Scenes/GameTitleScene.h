#pragma once
#include "IScene.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/DebugCamera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"
#include "Engine/2D/Sprite/Sprite.h"
#include <memory>

#include "Application/GameObject/Skydome.h"

class GameTitleScene : public IScene
{
public:
	static const int kTransitionTime = 60;

	GameTitleScene();

	~GameTitleScene();

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


	//title(モノクロファイター)のsprite
	std::unique_ptr<Sprite>titleSprite_ = nullptr;
	uint32_t titleTextureHandle_ = 0;
	Vector2 titleSpritePosition_ = { 0.0f,-25.0f };
	float titleSpriteMoveSpeed_ = 1.5f;
	int titleSpriteMoveTimer_ = 30;

	//title(操作用)のsprite
	std::unique_ptr<Sprite>titleUISprite_ = nullptr;
	uint32_t titleUITextureHandle_ = 0;

	//基本操作説明用のsprite
	std::unique_ptr<Sprite>generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	//攻撃操作説明用のsprite
	std::unique_ptr<Sprite>attackCommandListSprite_ = nullptr;
	uint32_t attackCommandListTextureHandle_ = 0;

	//sounds
	uint32_t titleSoundHandle_ = 0u;
	uint32_t selectSoundHandle_ = 0u;

	//何枚目のspriteが表示されているか
	int spriteCount_ = 0;

	//説明が開かれているか
	bool isOpen_ = false;

	//transition
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	//skydome
	std::unique_ptr<Skydome> skydome_;
};
