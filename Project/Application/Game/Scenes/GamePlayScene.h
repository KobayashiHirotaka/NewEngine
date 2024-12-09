/**
 * @file GamePlayScene.h
 * @brief プレイシーンの管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "IScene.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Engine/3D/Line/Line.h"
#include "Application/Game/InputLog/InputLog.h"
#include "Application/Game/CameraController/CameraController.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/GameObject/Character/Enemy/EnemyBullet.h"
#include "Application/GameObject/Skydome/Skydome.h"
#include "Application/GameObject/BackGround/BackGround.h"
#include "Application/Game/HitStop/HitStop.h"

class GamePlayScene : public IScene
{
public:
	//Transition用の時間
	static const int kTransitionTime = 60;

	//ラウンド移行の時間
	static int migrationTimer;

	//ラウンド開始までの時間
	static int roundStartTimer_;

	GamePlayScene();

	~GamePlayScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

	void ImGui()override;

	float Random(float min_value, float max_value);

	//数字の更新関数
	void UpdateNumberSprite();

	//勝敗を決める関数
	void HandleGameOutcome();

	//void HandleTransition();

	//ラウンド間でのトランジション
	void RoundTransition(int round);

private:
	TextureManager* textureManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//LevelLoader
	LevelLoader* levelLoarder_ = nullptr;

	//GameObjectManager
	Game3dObjectManager* game3dObjectManager_;

	//CollisionManager
	std::unique_ptr<CollisionManager> collisionManager_;

	//InputLog
	std::unique_ptr<InputLog> inputLog_;

	//HitStop
	std::unique_ptr<HitStop> hitStop_;

	//Camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	//CameraController
	std::unique_ptr<CameraController> cameraController_;

	//Timer用のSprite
	std::unique_ptr<Sprite>numberTensSprite_ = nullptr;
	std::unique_ptr<Sprite>numberOnesSprite_ = nullptr;
	uint32_t tensTextureHandle_;
	uint32_t onesTextureHandle_;

	//Round表示のSprite
	std::unique_ptr<Sprite>roundSprite_[3];
	uint32_t roundTextureHandle_[3];

	std::unique_ptr<Sprite>roundGetSprite_[4];
	uint32_t roundGetTextureHandle_;

	//試合開始時用のSprite(Fightの文字)
	std::unique_ptr<Sprite>fightSprite_ = nullptr;
	uint32_t fightTextureHandle_;

	//KO表示用Sprite
	std::unique_ptr<Sprite>koSprite_ = nullptr;
	uint32_t koTextureHandle_;

	//勝敗に関するSprite
	std::unique_ptr<Sprite>winSprite_ = nullptr;
	uint32_t winTextureHandle_;

	std::unique_ptr<Sprite>loseSprite_ = nullptr;
	uint32_t loseTextureHandle_;

	std::unique_ptr<Sprite>timeOverSprite_ = nullptr;
	uint32_t timeOverTextureHandle_;

	//操作説明用のSprite
	std::unique_ptr<Sprite>UICommandListSprite_ = nullptr;
	uint32_t UICommandListTextureHandle_ = 0;

	//基本操作説明用のSprite
	std::unique_ptr<Sprite>generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	//攻撃操作説明用のSprite
	std::unique_ptr<Sprite>attackCommandListSprite_[2];
	uint32_t attackCommandListTextureHandle_[2];

	//UI枠のSprite
	std::unique_ptr<Sprite>frameUISprite_ = nullptr;
	uint32_t frameUITextureHandle_ = 0;

	//半透明のSprite
	std::unique_ptr<Sprite>commandListBackSprite_ = nullptr;
	uint32_t commandListBackTextureHandle_ = 0;

	//何枚目のSpriteが表示されているか
	int spriteCount_ = 0;

	//操作説明が開かれているか
	bool isOpen_ = false;

	//時間
	int currentSeconds_;

	float frameTime = 1.0f / 60.0f;
	float elapsedTime = 0.0f;

	const int maxRoundStartTime_ = 100;
	const int halfRoundStartTime_ = 50;

	const int maxMigrationTime_ = 200;

	const int outComeTime_ = 150;
	const int testKoActiveTime_ = 0;

	//Sounds
	uint32_t selectSoundHandle_ = 0u;

	//Shake
	bool isShake_ = false;
	const int kShakeTime = 10;
	int shakeTimer_ = 0;
	Vector2 shakePower_ = { 0.9f,1.1f };

	//Transition
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	//モデルの骨を描画するかどうか
	bool isBoneDraw_ = true;

	//ラウンド
	int round_ = 1;

	//試合の結果
	bool isPlayerWin_ = false;
	bool isEnemyWin_ = false;
	bool isDrow_ = false;

	//キャラクターが勝っている回数
	int PlayerWinCount_ = 0;
	int EnemyWinCount_ = 0;

	//Player
	Player* player_;

	//Enemy
	Enemy* enemy_;

	//Skydome
	std::unique_ptr<Skydome> skydome_;

	//BackGround
	std::unique_ptr<BackGround> backGround_;

	//Skybox
	std::unique_ptr<Skybox> skybox_;
	WorldTransform skyboxWorldTransform_;

	bool isRoundTransition_ = false;
	int roundTransitionTimer_ = 150;

	bool isDebug_ = false;

	bool isKO_ = false;

	bool isTimeOver_ = false;

	//スティック操作対応
	const float value_ = 0.7f;
	int stickInputCooldown_ = 10;

	bool isFinisherStart_ = false;
	bool isFinisherEnd_ = false;
};
