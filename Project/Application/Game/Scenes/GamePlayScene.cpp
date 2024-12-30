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
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのインスタンスを取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//Audioのインスタンスを取得
	audio_ = Audio::GetInstance();

	//CollisionManagerのインスタンスを取得
	collisionManager_ = std::make_unique<CollisionManager>();

	//Game3dObjectManagerのインスタンスを取得
	game3dObjectManager_ = Game3dObjectManager::GetInstance();

	//Game3dObjectManagerの初期化
	game3dObjectManager_->Initialize();

	//Levelの読み込み
	levelLoarder_ = LevelLoader::GetInstance();
	levelLoarder_->LoadLevel("LevelData");

	//InputLogの生成、初期化
	inputLog_ = std::make_unique<InputLog>();
	inputLog_->Initialize();

	//HitStopの生成、初期化
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

	//BackGroundの生成、初期化
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize();

	//リソース
	UICommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/UICommandList.png");
	UICommandListSprite_.reset(Sprite::Create(UICommandListTextureHandle_, { 0.0f,0.0f }));

	//基本操作説明
	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayGeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明(コンボ)
	attackCommandListTextureHandle_[0] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], { 0.0f,0.0f }));

	//攻撃操作説明(必殺技)
	attackCommandListTextureHandle_[1] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList2.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));

	//ラウンド表示
	roundTextureHandle_[0] = TextureManager::LoadTexture("resource/images/Round1.png");
	roundTextureHandle_[1] = TextureManager::LoadTexture("resource/images/Round2.png");
	roundTextureHandle_[2] = TextureManager::LoadTexture("resource/images/Round3.png");

	roundSprite_[0].reset(Sprite::Create(roundTextureHandle_[0], { 0.0f, 0.0f }));
	roundSprite_[1].reset(Sprite::Create(roundTextureHandle_[1], { 0.0f, 0.0f }));
	roundSprite_[2].reset(Sprite::Create(roundTextureHandle_[2], { 0.0f, 0.0f }));

	//ラウンド取得数表示
	roundGetTextureHandle_ = TextureManager::LoadTexture("resource/images/RoundGet.png");

	const Vector2 kRoundGetPosition[4] = { { 400.0f, 70.0f }, { 480.0f, 70.0f }, { 800.0f, 70.0f }, { 720.0f, 70.0f } };

	roundGetSprite_[0].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[0]));
	roundGetSprite_[1].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[1]));
	roundGetSprite_[2].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[2]));
	roundGetSprite_[3].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[3]));

	//ラウンド開始時
	fightTextureHandle_ = TextureManager::LoadTexture("resource/images/FIGHT.png");

	fightSprite_.reset(Sprite::Create(fightTextureHandle_, { 0.0f, 0.0f }));

	//KO表示
	koTextureHandle_ = TextureManager::LoadTexture("resource/images/KO.png");

	koSprite_.reset(Sprite::Create(koTextureHandle_, { 0.0f, 0.0f }));

	//勝敗表示
	winTextureHandle_ = TextureManager::LoadTexture("resource/images/WIN.png");
	loseTextureHandle_ = TextureManager::LoadTexture("resource/images/LOSE.png");
	timeOverTextureHandle_ = TextureManager::LoadTexture("resource/images/TIMEOVER.png");

	winSprite_.reset(Sprite::Create(winTextureHandle_, { 0.0f, 0.0f }));
	loseSprite_.reset(Sprite::Create(loseTextureHandle_, { 0.0f, 0.0f }));
	timeOverSprite_.reset(Sprite::Create(timeOverTextureHandle_, { 0.0f, 0.0f }));

	//UIの枠
	frameUITextureHandle_ = TextureManager::LoadTexture("resource/images/frameUI.png");
	frameUISprite_.reset(Sprite::Create(frameUITextureHandle_, { 0.0f, 0.0f }));

	//数字
	tensTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");
	onesTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");

	const Vector2 kNumberTensPosition = { 590.0f, 0.0f };
	const Vector2 kNumberOnesPosition = { 630.0f, 0.0f };

	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, kNumberTensPosition));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, kNumberOnesPosition));

	//トランジション
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(transitionTextureSize_);

	//SE
	selectSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/Select.mp3");

#ifdef _ADJUSTMENT

	isDebug_ = true;

#endif // ADJUSTMENT

	//ラウンドごとの時間
	currentSeconds_ = kMaxRoundTime_;
	UpdateNumberSprite();

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
	//操作説明を開いていない場合
	if (!isOpen_)
	{
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
				if (player_->GetFinisherTimer() == kFinisherTime_)
				{
					currentSeconds_--;
					UpdateNumberSprite();

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

		//BackGroundの更新
		backGround_->Update();
	}

	//必殺技発動時のカメラ移動処理
	if (player_->GetIsFinisher() && player_->GetFinisherTimer() != kFinisherTime_)
	{
		isFinisherStart_ = true;
	}

	if (isFinisherStart_)
	{
		if (player_->GetIsDirectionRight())
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, player_->GetWorldPosition().x + kCameraFinisherOffsetY_, kCameraLerpSpeed_);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, -kCameraFinisherRotationY_, kCameraLerpSpeed_);
			cameraController_->GetCamera().UpdateMatrix();
		}
		else
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, player_->GetWorldPosition().x - kCameraFinisherOffsetY_, kCameraLerpSpeed_);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, kCameraFinisherRotationY_, kCameraLerpSpeed_);
			cameraController_->GetCamera().UpdateMatrix();
		}

		if (!player_->GetIsFinisherEffect())
		{
			isFinisherEnd_ = true;
			isFinisherStart_ = false;
		}
	}

	//必殺技終了時のカメラ移動処理
	if (isFinisherEnd_)
	{
		if (player_->GetIsDirectionRight())
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, cameraController_->GetCenter().x - kCameraEndCorrectionY_, kCameraLerpSpeed_);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, kCameraEndCorrectionY_, kCameraSmallLerpSpeed_);
			cameraController_->GetCamera().UpdateMatrix();

			if (cameraController_->GetCamera().translation_.x <= cameraController_->GetCenter().x && cameraController_->GetCamera().rotation_.y >= 0.0f)
			{
				cameraController_->GetCamera().translation_.x = cameraController_->GetCenter().x;
				cameraController_->GetCamera().rotation_.y = 0.0f;
				isFinisherEnd_ = false;
			}
		}
		else
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, cameraController_->GetCenter().x + kCameraEndCorrectionY_, kCameraLerpSpeed_);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, -kCameraEndCorrectionY_, kCameraSmallLerpSpeed_);
			cameraController_->GetCamera().UpdateMatrix();

			if (cameraController_->GetCamera().translation_.x >= cameraController_->GetCenter().x && cameraController_->GetCamera().rotation_.y <= 0.0f)
			{
				cameraController_->GetCamera().translation_.x = cameraController_->GetCenter().x;
				cameraController_->GetCamera().rotation_.y = 0.0f;
				isFinisherEnd_ = false;
			}
		}
	}

	//必殺技演出時ではない場合
	if (!isFinisherStart_ && !isFinisherEnd_)
	{
		//通常時のCameraControllerの更新
		cameraController_->Update(player_->GetWorldPosition(), enemy_->GetWorldPosition());
	}

	//勝ち負けの処理
	HandleGameOutcome();

	//操作説明の開閉
	if (input_->GetJoystickState())
	{
		//オプションボタンを押して操作説明を開く
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && !isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = true;
			spriteCount_ = CommandSpriteType::GeneralCommandSprite;
		}
		//Bボタンを押して操作説明を閉じる
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = false;
			spriteCount_ = CommandSpriteType::GeneralCommandSprite;
		}

		//操作説明が開いている場合
		if (isOpen_)
		{
			// スティック入力のクールダウンが終わっている場合のみ移動
			if (stickInputCooldown_ <= 0)
			{
				ChangeCommandSprite();
			}

			// クールダウンを減らす
			if (stickInputCooldown_ > 0)
			{
				stickInputCooldown_--;
			}
		}
	}

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

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(cameraController_->GetCamera());

	if (!isOpen_)
	{
		//Game3dObjectManagerの描画
		game3dObjectManager_->Draw(cameraController_->GetCamera());

		//BackGroundの描画
		backGround_->Draw(cameraController_->GetCamera());
	}

	Model::PostDraw();

	ParticleModel::PreDraw();

	if (GamePlayScene::sRoundStartTimer_ <= 0 && !isOpen_)
	{
		//Playerのparticle描画
		player_->DrawParticle(cameraController_->GetCamera());

		//Enemyのparticle描画
		enemy_->DrawParticle(cameraController_->GetCamera());
	}

	ParticleModel::PostDraw();

	Line::PreDraw();

	if (isDebug_ && !isOpen_)
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

	//ラウンド開始表示
	if (sRoundStartTimer_ <= kMaxRoundStartTime_ && sRoundStartTimer_ > kHalfRoundStartTime_)
	{
		if (round_ == kRoundOne_)
		{
			roundSprite_[0]->Draw();
		}
		else if (round_ == kRoundTwo_)
		{
			roundSprite_[1]->Draw();
		}
		else if (round_ == kRoundThree_)
		{
			roundSprite_[2]->Draw();
		}
	}

	if (sRoundStartTimer_ <= kHalfRoundStartTime_ && sRoundStartTimer_ > 0)
	{
		fightSprite_->Draw();
	}

	//ラウンド終了時の勝敗表示
	if (sMigrationTimer < kOutComeTime_ && sMigrationTimer > kKOConditionTime)
	{
	
		if (isPlayerWin_ && !isTimeOver_)
		{
			koSprite_->Draw();
		}

		if (!isPlayerWin_ && !isTimeOver_)
		{
			koSprite_->Draw();
		}

		if (isTimeOver_)
		{
			timeOverSprite_->Draw();
		}
	}

	if (sRoundStartTimer_ <= 0 && !isOpen_)
	{
		frameUISprite_->Draw();

		player_->DrawSprite();

		enemy_->DrawSprite();

		numberOnesSprite_->Draw();
		numberTensSprite_->Draw();

		if (playerWinCount_ >= kPlayerFirstWinCount_)
		{
			roundGetSprite_[1]->Draw();
		}

		if (playerWinCount_ >= kPlayerSecondWinCount_)
		{
			roundGetSprite_[0]->Draw();
		}


		if (enemyWinCount_ >= kEnemyFirstWinCount_)
		{
			roundGetSprite_[3]->Draw();
		}

		if (enemyWinCount_ >= kEnemySecondWinCount_)
		{
			roundGetSprite_[2]->Draw();
		}

	}

	//操作説明の描画
	if (isOpen_ && spriteCount_ == CommandSpriteType::GeneralCommandSprite)
	{
		//基本操作
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == CommandSpriteType::ComboAttackCommandSprite)
	{
		//コンボ攻撃
		attackCommandListSprite_[0]->Draw();
	}

	if (isOpen_ && spriteCount_ == CommandSpriteType::FinisherAttackCommandSprite)
	{
		//必殺技攻撃
		attackCommandListSprite_[1]->Draw();
	}
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	if (!isOpen_ && isDebug_)
	{
		inputLog_->Draw();
	}

	transitionSprite_->Draw();

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{

	
}

void GamePlayScene::ImGui()
{
	
}

void GamePlayScene::ChangeCommandSprite()
{
	//スプライトの移動に必用な定数
	const int kNextSprite = 1;
	const int kPreviousSprite = -1;

	//操作説明の移動処理
	if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > kValue_) &&
		spriteCount_ != CommandSpriteType::FinisherAttackCommandSprite)
	{
		//次の説明に進む
		ApplyCommandSprite(kNextSprite);
	}
	else if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -kValue_) &&
		spriteCount_ != CommandSpriteType::GeneralCommandSprite)
	{
		//前の説明に戻る
		ApplyCommandSprite(kPreviousSprite);
	}
}

void GamePlayScene::ApplyCommandSprite(int changeAmount)
{
	//スプライトの遷移用のベクター
	static const std::vector<CommandSpriteType> spriteOrder = {
		CommandSpriteType::GeneralCommandSprite,
		CommandSpriteType::ComboAttackCommandSprite,
		CommandSpriteType::FinisherAttackCommandSprite
	};

	//現在のスプライトインデックスを取得
	auto it = std::find(spriteOrder.begin(), spriteOrder.end(), static_cast<CommandSpriteType>(spriteCount_));
	if (it == spriteOrder.end()) return;

	size_t currentIndex = std::distance(spriteOrder.begin(), it);

	//次のスプライトインデックスを計算
	size_t nextIndex = (currentIndex + changeAmount + spriteOrder.size()) % spriteOrder.size();

	//スプライトを変更
	spriteCount_ = spriteOrder[nextIndex];

	//サウンド再生とスティックのクールダウンを設定
	audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
	stickInputCooldown_ = kStickInputCooldownTime_;
}

void GamePlayScene::UpdateNumberSprite()
{
	//時間表示の更新
	const int kDecimalBase = 10;
	int tensDigit = currentSeconds_ / kDecimalBase;
	int onesDigit = currentSeconds_ % kDecimalBase;

	tensTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(tensDigit) + ".png");
	onesTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(onesDigit) + ".png");

	numberTensSprite_->SetTexture(tensTextureHandle_);
	numberOnesSprite_->SetTexture(onesTextureHandle_);
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
		HandleRoundResult(player_->GetHP(), enemy_->GetHP(), currentSeconds_);
	}

	//トランジション
	const float deltaTime = 1.0f / kTransitionTime;
	if (!isTransitionEnd_)
	{
		transitionTimer_ += deltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= kTransitionEndAlpha_)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransitionStart_)
	{
		transitionTimer_ += deltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		//Playerが勝利したとき
		if (playerWinCount_ == kPlayerSecondWinCount_)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
			sceneManager_->ChangeScene("GameWinScene");
			return;
		}

		//Enemyが勝利したとき
		if (enemyWinCount_ == kEnemySecondWinCount_)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
			sceneManager_->ChangeScene("GameLoseScene");
			return;
		}
	}

	//ラウンド遷移の処理
	if (isRoundTransition_)
	{
		int nextRound = round_;

		if (isDrow_)
		{
			nextRound = kRoundThree_;
		}
		else
		{
			nextRound++;
		}

		if (nextRound > kRoundThree_)
		{
			nextRound = kRoundThree_;
		}

		RoundTransition(nextRound);
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
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
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
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);

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
			if (round_ != kRoundThree_)
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
			if (round_ != kRoundThree_)
			{
				playerWinCount_++;
				enemyWinCount_++;
			}

			isRoundTransition_ = true;
		}
	}
}

void GamePlayScene::RoundTransition(int round)
{
	if (isRoundTransition_)
	{
		//トランジションタイマーの処理
		roundTransitionTimer_--;

		//トランジション
		const float kLerpSpeed = 0.1f;
		const int kTransitionOffset = 10;
		if (roundTransitionTimer_ > kHalfkRoundTransitionTime_)
		{
			transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, kLerpSpeed);
			transitionSprite_->SetColor(transitionColor_);
		}
		else if (roundTransitionTimer_ <= kHalfkRoundTransitionTime_ - kTransitionOffset && roundTransitionTimer_ > 0)
		{
			if (playerWinCount_ == kPlayerSecondWinCount_ || enemyWinCount_ == kEnemySecondWinCount_)
			{
				isTransitionStart_ = true;
			}
			else
			{
				transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, kLerpSpeed);
				transitionSprite_->SetColor(transitionColor_);
			}
		}
		else if (roundTransitionTimer_ <= 0)
		{
			isRoundTransition_ = false;
			roundTransitionTimer_ = kRoundTransitionTime_;
		}

		//ラウンド間の初期化処理
		if (roundTransitionTimer_ == kHalfkRoundTransitionTime_)
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
			UpdateNumberSprite();

			sMigrationTimer = kMaxMigrationTime_;

			const float deltaTime = 1.0f / 60.0f;
			frameTime_ = deltaTime;
			elapsedTime_ = 0.0f;

			sRoundStartTimer_ = kMaxRoundStartTime_;

			//PostEffectの設定
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
		}
	}
}