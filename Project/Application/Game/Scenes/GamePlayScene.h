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
	//操作説明の種類
	enum class CommandSpriteType
	{
		//基本操作
		GeneralCommandSprite,

		//コンボ攻撃
		ComboAttackCommandSprite,

		//必殺技攻撃
		FinisherAttackCommandSprite,
	};

	//トランジション用の時間
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

private:

	/// <summary>数字の更新</summary>
	void UpdateNumberSprite();

	/// <summary>ゲーム全体の勝敗を決める</summary>
	void HandleGameOutcome();

	/// <summary>ゲーム結果を決める</summary>
	void HandleRoundResult(bool isRoundTransition, int playerHP, int enemyHP, int currentSeconds);

	/// <summary>Playerが勝ったときの処理</summary>
	void HandlePlayerWin(bool isTimeOver);

	/// <summary>Enemyが勝ったときの処理</summary>
	void HandleEnemyWin(bool isTimeOver);

	/// <summary>Drowだったときの処理</summary>
	void HandleDrow(bool isTimeOver);

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

	//Player
	Player* player_;

	//Enemy
	Enemy* enemy_;

	//Skydome
	std::unique_ptr<Skydome> skydome_;

	//BackGround
	std::unique_ptr<BackGround> backGround_;

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

	//何枚目のSpriteが表示されているか
	int spriteCount_ = 0;

	//操作説明が開かれているか
	bool isOpen_ = false;

	//時間
	//ラウンドの最大時間
	const int kMaxRoundTime_ = 5;
	
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

	//必殺技中の時間
	const int kFinisherTime_ = 120;

	//KO時間
	const int kKOConditionTime = 20;

	//サウンド
	uint32_t selectSoundHandle_ = 0u;
	const float volume_ = 1.0f;

	//トランジション
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector2 transitionTextureSize_ = { 1280.0f,720.0f };
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	
	//トランジションタイマー
	float transitionTimer_ = 0;

	//トランジション開始時のアルファ値
	const float kTransitionStartAlpha_ = 1.0f;

	//トランジション終了時のアルファ値
	const float kTransitionEndAlpha_ = 0.0f;

	//トランジション開始・終了フラグ
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	//ラウンド間のトランジション
	bool isRoundTransition_ = false;
	const int kRoundTransitionTime_ = 150;
	const int kHalfkRoundTransitionTime_ = kRoundTransitionTime_ / 2;
	int roundTransitionTimer_ = kRoundTransitionTime_;

	//モデルの骨を描画するかどうか
	bool isBoneDraw_ = true;

	//ラウンド
	static const int kRoundOne_ = 1;
	static const int kRoundTwo_ = 2;
	static const int kRoundThree_ = 3;

	int round_ = kRoundOne_;

	//試合の結果
	bool isPlayerWin_ = false;
	bool isEnemyWin_ = false;
	bool isDrow_ = false;

	//キャラクターが勝っている回数
	int playerWinCount_ = 0;
	int enemyWinCount_ = 0;

	//勝利カウント
	//Player
	const int kPlayerFirstWinCount_ = 1;   
	const int kPlayerSecondWinCount_ = 2; 

	//Enemy
	const int kEnemyFirstWinCount_ = 1;    
	const int kEnemySecondWinCount_ = 2;   

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

	//必殺技時のカメラ演出用のパラメータ
	//プレイヤーの位置からのカメラのオフセット
	const float kCameraFinisherOffsetY_ = 4.0f;

	//カメラ移動と回転の補間速度
	const float kCameraLerpSpeed_ = 0.2f;

	//必殺技発動時のカメラ回転角
	const float kCameraFinisherRotationY_ = 0.7f;

	//必殺技終了時のカメラ微調整オフセット
	const float kCameraEndCorrectionY_ = 0.1f;

	//微調整用補間速度
	const float kCameraSmallLerpSpeed_ = 0.1f;

	//微小な回転補正角度
	const float kCameraSmallRotationCorrection_ = 0.1f;
};
