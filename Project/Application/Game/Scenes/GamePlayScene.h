#pragma once
#include "IScene.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/GameObject/Character/Enemy/EnemyBullet.h"
#include "Application/GameObject/Skydome/Skydome.h"

class GamePlayScene : public IScene
{
public:
	static const int kTransitionTime = 60;

	static int migrationTimer;

	static float roundStartTimer_;

	GamePlayScene();

	~GamePlayScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

	void ImGui()override;

	void UpdateNumberSprite();

	float Random(float min_value, float max_value);

	void HandleGameOutcome();

private:
	//textureManager
	TextureManager* textureManager_ = nullptr;

	//modelManager
	ModelManager* modelManager_ = nullptr;

	//levelLoader
	LevelLoader* levelLoarder_ = nullptr;

	//gameObjectManager
	Game3dObjectManager* game3dObjectManager_;

	//input
	Input* input_ = nullptr;

	//audio
	Audio* audio_ = nullptr;

	//collisionManager
	std::unique_ptr<CollisionManager> collisionManager_;

	//camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	std::unique_ptr<Sprite>numberTensSprite_ = nullptr;
	std::unique_ptr<Sprite>numberOnesSprite_ = nullptr;
	uint32_t tensTextureHandle_;
	uint32_t onesTextureHandle_;

	std::unique_ptr<Sprite>roundSprite_[3];
	uint32_t roundTextureHandle_[3];

	std::unique_ptr<Sprite>fightSprite_ = nullptr;
	uint32_t fightTextureHandle_;

	std::unique_ptr<Sprite>roundGetSprite_[4];
	uint32_t roundGetTextureHandle_;

	std::unique_ptr<Sprite>winSprite_ = nullptr;
	uint32_t winTextureHandle_;

	std::unique_ptr<Sprite>loseSprite_ = nullptr;
	uint32_t loseTextureHandle_;

	std::unique_ptr<Sprite>drowSprite_ = nullptr;
	uint32_t drowTextureHandle_;

	std::unique_ptr<Sprite>UICommandListSprite_ = nullptr;
	uint32_t UICommandListTextureHandle_ = 0;

	std::unique_ptr<Sprite>generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	std::unique_ptr<Sprite>attackCommandListSprite_ = nullptr;
	uint32_t attackCommandListTextureHandle_ = 0;

	std::unique_ptr<Sprite>frameUISprite_ = nullptr;
	uint32_t frameUITextureHandle_ = 0;

	int currentSeconds_;

	float frameTime = 1.0f / 60.0f;  // 60FPSを仮定
	float elapsedTime = 0.0f;

	bool isPlayerWin_ = false;
	bool isDrow_ = false;

	uint32_t selectSoundHandle_ = 0u;

	int spriteCount_ = 0;
	bool isOpen_ = false;

	bool isShake_ = false;
	const int kShakeTime = 10;
	int shakeTimer_ = 0;
	Vector2 shakePower_ = { 0.9f,1.1f };

	//トランジション
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	bool isBoneDraw_ = true;

	int round_ = 1;
	int PlayerWinCount_ = 0;
	int EnemyWinCount_ = 0;

	//player
	Player* player_;

	//enemy
	Enemy* enemy_;

	//skydome
	std::unique_ptr<Skydome> skydome_;
};
