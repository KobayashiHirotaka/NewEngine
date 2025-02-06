/**
 * @file GamePlayScene.cpp
 * @brief プレイシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Application/GameObject/Character/BaseCharacter.h"
#include "Application/Game/GameTimer/GameTimer.h"
#include <cassert>

int GamePlayScene::sMigrationTimer = 200;
int GamePlayScene::sRoundStartTimer_ = 100;

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize()
{
	//TextureManagerのインスタンスを取得
	textureManager_ = Engine::TextureManager::GetInstance();

	//ModelManagerのインスタンスを取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスを取得
	input_ = Engine::Input::GetInstance();

	//Audioのインスタンスを取得
	audio_ = Engine::Audio::GetInstance();

	//AttackEditorのインスタンスを取得
	attackEditor_ = AttackEditor::GetInstance();

	//Game3dObjectManagerのインスタンスを取得、初期化
	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->Initialize();

	//CollisionManagerを生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//LevelDataのインスタンスを取得、読み込み
	levelLoarder_ = LevelLoader::GetInstance();
	levelLoarder_->LoadLevel("LevelData");

	//InputLogの生成、初期化
	inputLog_ = std::make_unique<InputLog>();
	inputLog_->Initialize();

	//HitStopの生成
	hitStop_ = std::make_unique<HitStop>();

	//CameraControllerの生成、初期化
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();

	//Playerの取得
	player_ = game3dObjectManager_->GetGameObject<Player>("Player");

	//Enemyの取得
	enemy_ = game3dObjectManager_->GetGameObject<Enemy>("Enemy");

	//EnemyとHitStopのセット
	player_->SetEnemy(enemy_);
	player_->SetHitStop(hitStop_.get());

	//PlayerとHitStopのセット
	enemy_->SetPlayer(player_);
	enemy_->SetHitStop(hitStop_.get());

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//UI生成、初期化
	gamePlaySceneUI_ = std::make_unique<GamePlaySceneUI>();
	gamePlaySceneUI_->Initialize();

	//操作説明の生成、初期化
	guide_ = std::make_unique<Guide>();
	guide_->Initialize();

	//SE
	selectSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Select.mp3");

	//Transition生成、初期化
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

#ifdef _ADJUSTMENT

	isDebug_ = true;

#endif // ADJUSTMENT

	//ラウンドごとの時間
	currentSeconds_ = kMaxRoundTime_;
	gamePlaySceneUI_->UpdateNumberSprite(currentSeconds_);

	//時間の初期設定
	sMigrationTimer = kMaxMigrationTime_;
	const float deltaTime = 1.0f / 60.0f;
	frameTime_ = deltaTime;
	elapsedTime_ = 0.0f;
	sRoundStartTimer_ = kMaxRoundStartTime_;

	//勝敗
	isPlayerWin_ = false;
	isDrow_ = false;

	//DebugCameraの初期化
	debugCamera_.Initialize();
};

void GamePlayScene::Update()
{
	//AttackEditorの更新
	attackEditor_->Update();

	//操作説明の開閉処理
	if (sRoundStartTimer_ <= 0)
	{
		guide_->Update();
	}

	if (guide_->GetIsChangedSprite())
	{
		audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
		guide_->SetIsChangedSprite(false);
	}

	//操作説明を開いている場合
	if (guide_->GetIsOpen())return;

	//ラウンド間の時間の処理
	sRoundStartTimer_--;

	if (sRoundStartTimer_ <= 0)
	{
		//時間経過を加算
		elapsedTime_ += GameTimer::GetDeltaTime();

		//タイムカウントを更新
		const float kTimeStep = 1.0f;
		if (currentSeconds_ > 0 && elapsedTime_ >= kTimeStep && sMigrationTimer == kMaxMigrationTime_)
		{
			if (player_->GetFinisherTimer() == kFinisherTime_ && !isDebug_)
			{
				currentSeconds_--;
				gamePlaySceneUI_->UpdateNumberSprite(currentSeconds_);

				//elapsedTimeをリセット
				elapsedTime_ = 0.0f;
			}
		}
	}

	//KOの処理
	if (player_->GetIsKO() && sMigrationTimer > kKOConditionTime)
	{
		isKO_ = true;
	}
	else if (enemy_->GetIsKO() && sMigrationTimer > kKOConditionTime)
	{
		isKO_ = true;
	}
	else
	{
		isKO_ = false;

		enemy_->SetIsKO(false);
	}

	//KOではない場合
	if (!isKO_)
	{
		//Game3dObjectManagerの更新
		game3dObjectManager_->Update();
	}

	//Skydomeの更新
	skydome_->Update();

	//必殺技発動時のカメラ移動処理
	if (player_->GetAttackData().isFinisher && player_->GetFinisherTimer() != kFinisherTime_)
	{
		isFinisherStart_ = true;
	}

	if (isFinisherStart_)
	{
		cameraController_->StartFinisherCamera(player_->GetCharacterState().direction, player_->GetWorldPosition().x);

		if (!player_->GetIsFinisherEffect() || player_->GetIsDown())
		{
			isFinisherEnd_ = true;
			isFinisherStart_ = false;
		}
	}

	//必殺技終了時のカメラ移動処理
	if (isFinisherEnd_)
	{
		cameraController_->EndFinisherCamera(player_->GetCharacterState().direction, isFinisherEnd_);
	}

	//必殺技演出時ではない場合
	if (!isFinisherStart_ && !isFinisherEnd_)
	{
		//通常時のCameraControllerの更新
		cameraController_->Update(player_->GetWorldPosition(), enemy_->GetWorldPosition());
	}

	//勝ち負けの処理
	HandleGameOutcome();

	//当たり判定
	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_->GetCollider());

	collisionManager_->AddCollider(enemy_->GetCollider());

	for (const auto& bullet : player_->GetBullets())
	{
		collisionManager_->AddCollider(bullet->GetCollider());
	}

	for (const auto& bullet : enemy_->GetBullets())
	{
		collisionManager_->AddCollider(bullet->GetCollider());
	}

	collisionManager_->CheckAllCollision();

	//HitStopの更新
	hitStop_->Update();

	//InputLogの更新
	inputLog_->Update();

	//DebugCameraの更新
	debugCamera_.Update();

#ifdef _ADJUSTMENT

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

#endif 

	//DebugCameraの処理
	if (isDebugCamera_)
	{
		camera_.matView_ = debugCamera_.GetCamera().matView_;
		camera_.matProjection_ = debugCamera_.GetCamera().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		//カメラの更新
		camera_.UpdateMatrix();
	}
};

void GamePlayScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	Engine::PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(cameraController_->GetCamera());

	if (!guide_->GetIsOpen())
	{
		//Game3dObjectManagerの描画
		game3dObjectManager_->Draw(cameraController_->GetCamera());
	}

	Model::PostDraw();

	ParticleModel::PreDraw();

	if (GamePlayScene::sRoundStartTimer_ <= 0 && !guide_->GetIsOpen())
	{
		//Playerのparticle描画
		player_->DrawParticle(cameraController_->GetCamera());

		//Enemyのparticle描画
		enemy_->DrawParticle(cameraController_->GetCamera());
	}

	ParticleModel::PostDraw();

	Line::PreDraw();

	if (isDebug_ && !guide_->GetIsOpen())
	{
		//Playerの当たり判定描画
		player_->DrawCollision(cameraController_->GetCamera());

		//Enemyの当たり判定描画
		enemy_->DrawCollision(cameraController_->GetCamera());
	}

	Line::PostDraw();

	Model::PreDrawBone();

#ifdef _ADJUSTMENT

	////playerのbone描画
	//player_->DrawBone(cameraController_->GetCamera());

	////enemyのbone描画
	//enemy_->DrawBone(cameraController_->GetCamera());

#endif

	Model::PostDrawBone();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//ラウンド開始時の描画
	if (sRoundStartTimer_ <= kMaxRoundStartTime_ && sRoundStartTimer_ > kHalfRoundStartTime_)
	{
		gamePlaySceneUI_->RoundNumberDraw(round_);
	}

	if (sRoundStartTimer_ <= kHalfRoundStartTime_ && sRoundStartTimer_ > 0)
	{
		gamePlaySceneUI_->RoundStartDraw();
	}

	//ラウンド終了時の描画
	if (sMigrationTimer < kOutComeTime_ && sMigrationTimer > kKOConditionTime)
	{
		gamePlaySceneUI_->RoundEndDraw(isTimeOver_, isPlayerWin_);
	}

	//プレイ中のUI描画
	if (sRoundStartTimer_ <= 0)
	{
		if (!guide_->GetIsOpen())
		{
			player_->DrawSprite();

			enemy_->DrawSprite();

			gamePlaySceneUI_->Draw();

			gamePlaySceneUI_->RoundGetDraw(playerWinCount_, enemyWinCount_);
		}
		else
		{
			guide_->Draw();
		}
	}
	
	Sprite::PostDraw();

	Engine::PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//入力履歴の描画
	if (!guide_->GetIsOpen() && isDebug_)
	{
		inputLog_->Draw();
	}

	//Transition用Spriteの描画
	transition_->Draw();

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{

	
}

void GamePlayScene::ImGui()
{
	
}

float GamePlayScene::Random(float min_value, float max_value)
{
	//乱数を生成
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min_value, max_value);

	return dis(gen);
}

void GamePlayScene::HandleGameOutcome()
{
	//ラウンドごとの勝敗処理
	if (!isRoundTransition_)
	{
		HandleRoundResult(player_->GetBaseData().hp_, enemy_->GetBaseData().hp_, currentSeconds_);
	}
	
	//Transition終了処理
	transition_->EndSceneTransition(isTransitionEnd_);

	//Playerが勝利したとき
	if (playerWinCount_ == kMaxWinCount_)
	{
		isTransitionStart_ = true;
		transition_->StartSceneTransition(isTransitionStart_, sceneManager_, "GameWinScene");
		return;
	}

	//Enemyが勝利したとき
	if (enemyWinCount_ == kMaxWinCount_)
	{
		isTransitionStart_ = true;
		transition_->StartSceneTransition(isTransitionStart_, sceneManager_, "GameLoseScene");
		return;
	}

	//ラウンド遷移の処理
	if (isRoundTransition_)
	{
		int nextRound = round_;

		if (isDrow_)
		{
			nextRound = kMaxRound_;
		}
		else
		{
			nextRound++;
		}

		if (nextRound > kMaxRound_)
		{
			nextRound = kMaxRound_;
		}

		ChangeRound(nextRound);
	}
}

void GamePlayScene::HandleRoundResult(int playerHP, int enemyHP, int currentSeconds)
{
	//時間切れ
	if (currentSeconds <= 0)
	{
		if (abs(enemyHP) < abs(playerHP))
		{
			//Playerの勝利
			HandlePlayerWin(true); 
		}
		else if (abs(enemyHP) > abs(playerHP))
		{
			//Enemyの勝利
			HandleEnemyWin(true); 
		}
		else
		{
			//引き分け
			HandleDrow(true); 
		}
		return; 
	}

	if (!isRoundTransition_)
	{
		//Playerの勝敗判定
		if (enemyHP <= 0 && playerHP < 0)
		{
			HandlePlayerWin(false);
		}

		//Enemyの勝敗判定
		if (playerHP >= 0 && enemyHP > 0)
		{
			HandleEnemyWin(false);
		}

		//引き分け
		if (playerHP >= 0 && enemyHP <= 0)
		{
			HandleDrow(false);
		}
	}
}

void GamePlayScene::HandlePlayerWin(bool isTimeOver)
{
	sMigrationTimer--;
	isPlayerWin_ = true;

	//Playerが勝ったとき
	if (!isTimeOver)
	{
		//倒した
		Engine::PostProcess::GetInstance()->SetIsGrayScaleActive(true);

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetAttackData().isFinisherSecondAttack && !player_->GetAttackData().isTackle)
		{
			playerWinCount_++;
			isRoundTransition_ = true;
		}
	}
	else
	{
		//時間切れ
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			playerWinCount_++;
			isRoundTransition_ = true;
		}
	}
}

void GamePlayScene::HandleEnemyWin(bool isTimeOver)
{
	sMigrationTimer--;
	isPlayerWin_ = false;

	//Enemyが勝ったとき
	if (!isTimeOver)
	{
		//倒した
		Engine::PostProcess::GetInstance()->SetIsGrayScaleActive(true);

		if (sMigrationTimer < kKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			enemyWinCount_++;
			isRoundTransition_ = true;
		}
	}
	else
	{
		//時間切れ
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			enemyWinCount_++;
			isRoundTransition_ = true;
		}
	}
}

void GamePlayScene::HandleDrow(bool isTimeOver)
{
	sMigrationTimer--;
	isDrow_ = true;

	//引き分けのとき
	if (!isTimeOver)
	{
		//相打ち
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			if (round_ != kMaxRound_)
			{
				playerWinCount_++;
				enemyWinCount_++;
			}

			isRoundTransition_ = true;
		}
	}
	else
	{
		//時間切れ
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			if (round_ != kMaxRound_)
			{
				playerWinCount_++;
				enemyWinCount_++;
			}

			isRoundTransition_ = true;
		}
	}
}

void GamePlayScene::ChangeRound(int round)
{
	//ラウンド間のトランジション処理
	transition_->RoundTransition(isRoundTransition_);

	//ラウンド間の初期化処理
	if (transition_->GetIsRoundTransitioning())
	{
		//勝敗を元に戻す
		isPlayerWin_ = false;
		isDrow_ = false;
		isTimeOver_ = false;
		round_ = round;

		//キャラクターのリセット
		player_->Reset();
		enemy_->Reset();

		//時間の設定
		currentSeconds_ = kMaxRoundTime_;
		gamePlaySceneUI_->UpdateNumberSprite(currentSeconds_);

		sMigrationTimer = kMaxMigrationTime_;

		const float deltaTime = 1.0f / 60.0f;
		frameTime_ = deltaTime;
		elapsedTime_ = 0.0f;

		sRoundStartTimer_ = kMaxRoundStartTime_;

		//PostEffectの設定
		Engine::PostProcess::GetInstance()->SetIsGrayScaleActive(false);
		Engine::PostProcess::GetInstance()->SetIsVignetteActive(false);

		transition_->SetIsRoundTransitioning(false);
	}
}