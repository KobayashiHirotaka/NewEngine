/**
 * @file GamePlayScene.h
 * @brief プレイシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "IScene.h"
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
	static int sMigrationTimer;

	//ラウンド開始までの時間
	static int sRoundStartTimer_;

	/// <summary>コンストラクタ</summary>
	GamePlayScene();

	/// <summary>デストラクタ</summary>
	~GamePlayScene();

	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

	/// <summary>終了</summary>
	void Finalize()override;

	/// <summary>ImGui</summary>
	void ImGui()override;

	/// <summary>ランダム</summary>
	float Random(float min_value, float max_value);

	/// <summary>数字の更新</summary>
	void UpdateNumberSprite();

	/// <summary>勝敗を決める</summary>
	void HandleGameOutcome();

	/// <summary>ラウンド間でのトランジション</summary>
	void RoundTransition(int round);

	//void HandleTransition();

private:
	//TextureManager
	TextureManager* textureManager_ = nullptr;

	//ModelManager
	ModelManager* modelManager_ = nullptr;

	//Input
	Input* input_ = nullptr;

	//Audio
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
	//現在の時間
	int currentSeconds_ = 0;

	//ゲーム中の時間
	float frameTime_ = 1.0f / 60.0f;
	float elapsedTime_ = 0.0f;

	//ラウンド開始までの時間
	const int kMaxRoundStartTime_ = 100;
	const int kHalfRoundStartTime_ = kMaxRoundStartTime_ / 2;

	//ラウンド切り替えの時間
	const int kMaxMigrationTime_ = 200;

	//勝敗表示用の時間
	const int kOutComeTime_ = 150;
	const int kKoActiveTime_ = 0;

	//Sounds
	uint32_t selectSoundHandle_ = 0u;

	//Shake
	bool isShake_ = false;
	const int kShakeTime_ = 10;
	int shakeTimer_ = 0;
	Vector2 shakePower_ = { 0.9f,1.1f };

	//Transition
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;
	bool isRoundTransition_ = false;
	int roundTransitionTimer_ = 150;

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

	//デバッグかどうか
	bool isDebug_ = false;

	//どちらかがKOしているか
	bool isKO_ = false;

	//タイムオーバーかどうか
	bool isTimeOver_ = false;

	//スティック操作対応
	const float kValue_ = 0.7f;
	int stickInputCooldown_ = 10;

	//必殺技の開始・終了
	bool isFinisherStart_ = false;
	bool isFinisherEnd_ = false;
};
