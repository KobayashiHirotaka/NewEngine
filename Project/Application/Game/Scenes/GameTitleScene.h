#pragma once
#include "IScene.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Application/GameObject/Skydome/Skydome.h"

class GameTitleScene : public IScene
{
public:
	//Transition用の時間
	static const int kTransitionTime = 60;

	GameTitleScene();

	~GameTitleScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

	void ImGui()override;

private:
	//タイトルの文字を動かす
	void AnimationTitle();

private:
	TextureManager* textureManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//Camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	//タイトル(モノクロファイター)のSprite
	std::unique_ptr<Sprite>titleSprite_ = nullptr;
	uint32_t titleTextureHandle_ = 0;
	Vector2 titleSpritePosition_ = { 0.0f,-25.0f };
	float titleSpriteMoveSpeed_ = 1.5f;
	int titleSpriteMoveTimer_ = 30;

	//タイトル(操作用)のSprite
	std::unique_ptr<Sprite>titleUISprite_ = nullptr;
	uint32_t titleUITextureHandle_ = 0;

	//基本操作説明用のSprite
	std::unique_ptr<Sprite>generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	//攻撃操作説明用のSprite
	std::unique_ptr<Sprite>attackCommandListSprite_ = nullptr;
	uint32_t attackCommandListTextureHandle_ = 0;

	//Sounds
	uint32_t titleSoundHandle_ = 0u;
	uint32_t selectSoundHandle_ = 0u;

	//何枚目のSpriteが表示されているか
	int spriteCount_ = 0;

	//操作説明が開かれているか
	bool isOpen_ = false;

	//Transition
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	//Skydome
	std::unique_ptr<Skydome> skydome_;

	//Skybox
	std::unique_ptr<Skybox> skybox_;
	WorldTransform skyboxWorldTransform_;
};
