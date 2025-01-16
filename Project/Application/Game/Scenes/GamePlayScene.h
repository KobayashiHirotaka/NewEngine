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
#include "Application/Game/AttackEditor/AttackEditor.h"
#include "Application/Game/Scenes/GamePlaySceneUI.h"

class GamePlayScene : public IScene
{
public:
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
	/// <summary>ゲーム全体の勝敗を管理する</summary>
	void HandleGameOutcome();

	/// <summary>ラウンドの勝敗を判定する</summary>
	void HandleRoundResult(int playerHP, int enemyHP, int currentSeconds);

	/// <summary>Playerが勝ったときの処理</summary>
	void HandlePlayerWin(bool isTimeOver);

	/// <summary>Enemyが勝ったときの処理</summary>
	void HandleEnemyWin(bool isTimeOver);

	/// <summary>Drowだったときの処理</summary>
	void HandleDrow(bool isTimeOver);

	/// <summary>ラウンドの変更処理</summary>
	void ChangeRound(int round);

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

	//AttackEditor
	AttackEditor* attackEditor_ = nullptr;

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

	//時間
	//ラウンドの最大時間
	const int kMaxRoundTime_ = 99;
	
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

	//モデルの骨を描画するかどうか
	bool isBoneDraw_ = true;

	//ラウンド
	bool isRoundTransition_ = false;
	const int kMaxRound_ = 3;
	int round_ = 1;

	//試合の結果
	bool isPlayerWin_ = false;
	bool isEnemyWin_ = false;
	bool isDrow_ = false;

	//キャラクターが勝っている回数
	int playerWinCount_ = 0;
	int enemyWinCount_ = 0;
	const int kMaxWinCount_ = 2;

	//デバッグかどうか
	bool isDebug_ = false;

	//どちらかがKOしているか
	bool isKO_ = false;

	//タイムオーバーかどうか
	bool isTimeOver_ = false;

	//必殺技の開始・終了
	bool isFinisherStart_ = false;
	bool isFinisherEnd_ = false;

	//GamePlaySceneUI
	std::unique_ptr<GamePlaySceneUI> gamePlaySceneUI_;
};
