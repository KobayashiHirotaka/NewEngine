/**
 * @file GamePlayScene.cpp
 * @brief プレイシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Application/GameObject/Character/ICharacter.h"
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

	//クールダウンタイム
	const int kStickInputCooldownTime = 10;

	//操作説明の開閉
	if (input_->GetJoystickState())
	{
		//オプションボタンを押して操作説明を開く
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && !isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = true;
			spriteCount_ = static_cast<int>(CommandSpriteType::GeneralCommandSprite);
		}
		//Bボタンを押して操作説明を閉じる
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = false;
			spriteCount_ = static_cast<int>(CommandSpriteType::GeneralCommandSprite);
		}

		//操作説明が開いている場合
		if (isOpen_)
		{
			//右の方向キーかスティック右入力で次の説明へ
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) || (input_->GetLeftStickX() > kValue_ && stickInputCooldown_ <= 0))
			{
				if (spriteCount_ != static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite))
				{
					if (spriteCount_ == static_cast<int>(CommandSpriteType::GeneralCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::ComboAttackCommandSprite);
					}
					else if (spriteCount_ == static_cast<int>(CommandSpriteType::ComboAttackCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite);
					}

					audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
					stickInputCooldown_ = kStickInputCooldownTime;
				}
			}
			//左の方向キーかスティック左入力で前の説明に戻る
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) || (input_->GetLeftStickX() < -kValue_ && stickInputCooldown_ <= 0))
			{
				if (spriteCount_ != static_cast<int>(CommandSpriteType::GeneralCommandSprite))
				{
					if (spriteCount_ == static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::ComboAttackCommandSprite);
					}
					else if (spriteCount_ == static_cast<int>(CommandSpriteType::ComboAttackCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::GeneralCommandSprite);
					}

					audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
					stickInputCooldown_ = kStickInputCooldownTime;
				}
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
	collisionManager_->AddCollider(player_);

	collisionManager_->AddCollider(enemy_);

	for (const auto& bullet : enemy_->GetBullets())
	{
		collisionManager_->AddCollider(bullet);
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

		//Enemyの弾の描画
		enemy_->DrawBullet(cameraController_->GetCamera());

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

		if (PlayerWinCount_ >= kPlayerFirstWinCount_)
		{
			roundGetSprite_[1]->Draw();
		}

		if (PlayerWinCount_ >= kPlayerSecondWinCount_)
		{
			roundGetSprite_[0]->Draw();
		}


		if (EnemyWinCount_ >= kEnemyFirstWinCount_)
		{
			roundGetSprite_[3]->Draw();
		}

		if (EnemyWinCount_ >= kEnemySecondWinCount_)
		{
			roundGetSprite_[2]->Draw();
		}

	}

	//操作説明の描画
	if (isOpen_ && spriteCount_ == static_cast<int>(CommandSpriteType::GeneralCommandSprite))
	{
		//基本操作
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == static_cast<int>(CommandSpriteType::ComboAttackCommandSprite))
	{
		//コンボ攻撃
		attackCommandListSprite_[0]->Draw();
	}

	if (isOpen_ && spriteCount_ == static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite))
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

//TODO:長いので簡潔にする(冬休み)
void GamePlayScene::HandleGameOutcome()
{
	//Playerが勝ったとき
	if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == kRoundOne_ && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == kRoundTwo_ && PlayerWinCount_ == kPlayerFirstWinCount_ && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == kRoundTwo_ && PlayerWinCount_ == 0 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == kRoundThree_ && PlayerWinCount_ == kPlayerFirstWinCount_ && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}

	//時間切れ
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == kRoundOne_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == kRoundTwo_ && PlayerWinCount_ == kPlayerFirstWinCount_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == kRoundTwo_ && PlayerWinCount_ == 0 && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == kRoundThree_ && PlayerWinCount_ == kPlayerFirstWinCount_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}

	if (PlayerWinCount_ == kPlayerSecondWinCount_)
	{
		isTransitionStart_ = true;
	}

	//Enemyが勝ったとき
	if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == kRoundOne_ && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = false;

		if (sMigrationTimer < kKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == kRoundTwo_ && EnemyWinCount_ == kEnemyFirstWinCount_ && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = false;

		if (sMigrationTimer < kKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == kRoundTwo_ && EnemyWinCount_ == 0 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = false;

		if (sMigrationTimer < kKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == kRoundThree_ && EnemyWinCount_ == kEnemyFirstWinCount_ && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		sMigrationTimer--;
		isPlayerWin_ = false;

		if (sMigrationTimer < kKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}

	//時間切れ
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == kRoundOne_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == kRoundTwo_ && EnemyWinCount_ == kEnemyFirstWinCount_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == kRoundTwo_ && EnemyWinCount_ == 0 && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == kRoundThree_ && EnemyWinCount_ == kEnemyFirstWinCount_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}

	if (EnemyWinCount_ == kEnemySecondWinCount_)
	{
		if (isTransitionStart_ == false && isTransitionEnd_ == true)
		{
			isTransitionStart_ = true;
		}
	}

	//時間切れ(ドロー)
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && round_ == kRoundOne_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isDrow_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && round_ == kRoundThree_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isDrow_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && PlayerWinCount_ == kPlayerFirstWinCount_ && EnemyWinCount_ == 0 && round_ == kRoundTwo_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && EnemyWinCount_ == kEnemyFirstWinCount_ && PlayerWinCount_ == 0 && round_ == kRoundTwo_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && PlayerWinCount_ == kPlayerFirstWinCount_ && EnemyWinCount_ == 0 && round_ == kRoundThree_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && EnemyWinCount_ == kEnemyFirstWinCount_ && PlayerWinCount_ == 0 && round_ == kRoundThree_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (sMigrationTimer < kKoActiveTime_)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}

	//相打ち
	if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && round_ == kRoundOne_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isDrow_ = true;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && round_ == kRoundThree_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isDrow_ = true;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = kPlayerFirstWinCount_;
			EnemyWinCount_ = kEnemyFirstWinCount_;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && PlayerWinCount_ == kPlayerFirstWinCount_ && EnemyWinCount_ == 0 && round_ == kRoundTwo_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && EnemyWinCount_ == kEnemyFirstWinCount_ && PlayerWinCount_ == 0 && round_ == kRoundTwo_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && PlayerWinCount_ == kPlayerFirstWinCount_ && EnemyWinCount_ == 0 && round_ == kRoundThree_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = true;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = kPlayerSecondWinCount_;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && EnemyWinCount_ == kEnemyFirstWinCount_ && PlayerWinCount_ == 0 && round_ == kRoundThree_ && !isRoundTransition_)
	{
		sMigrationTimer--;
		isPlayerWin_ = false;

		if (sMigrationTimer < kKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = kEnemySecondWinCount_;
		}
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

		//Playerが勝利した場合
		if (PlayerWinCount_ == kPlayerSecondWinCount_)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
			sceneManager_->ChangeScene("GameWinScene");
			return;
		}

		//Enemyが勝利した場合
		if (EnemyWinCount_ == kEnemySecondWinCount_)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
			sceneManager_->ChangeScene("GameLoseScene");
			return;
		}
	}

	int roundCount = kRoundOne_;

	if (isRoundTransition_)
	{
		if (isDrow_)
		{
			roundCount = kRoundThree_;
		}
		else if (round_ == kRoundOne_)
		{
			roundCount = kRoundTwo_;
		}
		else if (round_ == kRoundTwo_)
		{
			roundCount = kRoundThree_;
		}
		else if (round_ == kRoundThree_)
		{
			roundCount = kRoundThree_;
		}

		RoundTransition(roundCount);
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
			transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, kLerpSpeed);
			transitionSprite_->SetColor(transitionColor_);
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