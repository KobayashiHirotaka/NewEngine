#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Application/GameObject/Character/ICharacter.h"
#include "Application/Game/GameTimer/GameTimer.h"
#include <cassert>

/**
 * @file GamePlayScene.cpp
 * @brief プレイシーンの管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

int GamePlayScene::migrationTimer = 200;
int GamePlayScene::roundStartTimer_ = 100;

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize()
{
	//TextureManagerのinstance
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//Inputのinstance
	input_ = Input::GetInstance();

	//Audioのinstance
	audio_ = Audio::GetInstance();

	//CollisionManagerのinstance
	collisionManager_ = std::make_unique<CollisionManager>();

	//Game3dObjectManagerのinstance
	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->Initialize();

	//Levelの読み込み
	levelLoarder_ = LevelLoader::GetInstance();
	levelLoarder_->LoadLevel("LevelData");

	//InputLogの生成
	inputLog_ = std::make_unique<InputLog>();
	inputLog_->Initialize();

	//HitStopの生成
	hitStop_ = std::make_unique<HitStop>();

	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();

	//Playerの生成、初期化
	player_ = game3dObjectManager_->GetGameObject<Player>("Player");

	//Enemyの生成、初期化
	enemy_ = game3dObjectManager_->GetGameObject<Enemy>("Enemy");

	player_->SetEnemy(enemy_);
	player_->SetHitStop(hitStop_.get());

	enemy_->SetPlayer(player_);
	enemy_->SetHitStop(hitStop_.get());

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//Skyboxの生成、初期化
	skybox_.reset(Skybox::Create());

	//BackGroundの生成、初期化
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize();

	skyboxWorldTransform_.Initialize();
	skyboxWorldTransform_.scale = { 500.0f, 500.0f, 500.0f };

	//リソース
	UICommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/UICommandList.png");
	UICommandListSprite_.reset(Sprite::Create(UICommandListTextureHandle_, { 0.0f,0.0f }));

	//基本操作説明
	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayGeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明
	attackCommandListTextureHandle_[0] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], { 0.0f,0.0f }));

	attackCommandListTextureHandle_[1] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList2.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));

	commandListBackTextureHandle_ = TextureManager::LoadTexture("resource/images/CommandListBack.png");
	commandListBackSprite_.reset(Sprite::Create(commandListBackTextureHandle_, { 0.0f,0.0f }));

	//ラウンド表示
	roundTextureHandle_[0] = TextureManager::LoadTexture("resource/images/Round1.png");
	roundTextureHandle_[1] = TextureManager::LoadTexture("resource/images/Round2.png");
	roundTextureHandle_[2] = TextureManager::LoadTexture("resource/images/Round3.png");

	roundSprite_[0].reset(Sprite::Create(roundTextureHandle_[0], { 0.0f, 0.0f }));
	roundSprite_[1].reset(Sprite::Create(roundTextureHandle_[1], { 0.0f, 0.0f }));
	roundSprite_[2].reset(Sprite::Create(roundTextureHandle_[2], { 0.0f, 0.0f }));

	roundGetTextureHandle_ = TextureManager::LoadTexture("resource/images/RoundGet.png");

	roundGetSprite_[0].reset(Sprite::Create(roundGetTextureHandle_, { 400.0f, 70.0f }));
	roundGetSprite_[1].reset(Sprite::Create(roundGetTextureHandle_, { 480.0f, 70.0f }));
	roundGetSprite_[2].reset(Sprite::Create(roundGetTextureHandle_, { 800.0f, 70.0f }));
	roundGetSprite_[3].reset(Sprite::Create(roundGetTextureHandle_, { 720.0f, 70.0f }));

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

	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, { 590.0f, 0.0f }));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, { 630.0f, 0.0f }));

	//トランジション
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	//SE
	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");

#ifdef _ADJUSTMENT

	isDebug_ = true;

#endif // ADJUSTMENT

	//ラウンドごとの時間
	currentSeconds_ = 99;
	UpdateNumberSprite();

	migrationTimer = maxMigrationTime_;

	frameTime = 1.0f / 60.0f;
	elapsedTime = 0.0f;

	roundStartTimer_ = maxRoundStartTime_;

	//勝敗
	isPlayerWin_ = false;
	isDrow_ = false;

	//DebugCameraの初期化
	debugCamera_.Initialize();
};

void GamePlayScene::Update()
{
	if (!isOpen_)
	{
		//ラウンド間の時間の処理
		roundStartTimer_--;

		if (roundStartTimer_ <= 0)
		{
			//時間経過を加算
			elapsedTime += GameTimer::GetDeltaTime();

			//タイムカウントを更新
			if (currentSeconds_ > 0 && elapsedTime >= 1.0f && migrationTimer == maxMigrationTime_)
			{
				if (player_->GetFinisherTimer() == 120)
				{
					currentSeconds_--;
					UpdateNumberSprite();

					//elapsedTimeをリセット
					elapsedTime = 0.0f;
				}
			}
		}

		if (player_->GetIsKO() && migrationTimer > 20)
		{
			isKO_ = true;
		}
		else if (enemy_->GetIsKO() && migrationTimer > 20)
		{
			isKO_ = true;
		}
		else
		{
			isKO_ = false;

			enemy_->SetIsKO(false);
		}

		if (!isKO_)
		{
			//Game3dObjectManagerの更新
			game3dObjectManager_->Update();
		}

		//Skydomeの更新
		skydome_->Update();

		//BackGroundの更新
		backGround_->Update();

		//シェイク
		if ((player_->GetIsShake() || enemy_->GetIsShake()) && !isPlayerWin_ && roundStartTimer_ <= 0)
		{
			isShake_ = true;
			shakeTimer_ = kShakeTime;
			//backGround_->SetStateNum(1);
		}

		if (isShake_)
		{
			cameraController_->GetCamera().translation_.y = Random(shakePower_.x, shakePower_.y);
			cameraController_->GetCamera().UpdateMatrix();

			if (--shakeTimer_ < 0)
			{
				isShake_ = false;
			}
		}
	}

	////PostEffectの値変更
	//if (player_->GetHP() >= -25.0f)
	//{
	//	PostProcess::GetInstance()->SetIsVignetteActive(true);
	//}
	//else
	//{
	//	PostProcess::GetInstance()->SetIsVignetteActive(false);
	//}

	//必殺技発動時のカメラ移動処理
	if (player_->GetIsFinisher() && player_->GetFinisherTimer() != 120)
	{
		isFinisherStart_ = true;
	}

	if (isFinisherStart_)
	{
		if (player_->GetIsDirectionRight())
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, player_->GetWorldPosition().x + 4.0f, 0.2f);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, -0.7f, 0.2f);
			cameraController_->GetCamera().UpdateMatrix();
		}
		else
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, player_->GetWorldPosition().x - 4.0f, 0.2f);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, 0.7f, 0.2f);
			cameraController_->GetCamera().UpdateMatrix();
		}

		if (!player_->GetIsFinisherEffect())
		{
			isFinisherEnd_ = true;
			isFinisherStart_ = false;
		}
	}

	if (isFinisherEnd_)
	{
		if (player_->GetIsDirectionRight())
		{
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, cameraController_->GetCenter().x - 0.1f, 0.2f);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, 0.1f, 0.1f);
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
			cameraController_->GetCamera().translation_.x = Lerp(cameraController_->GetCamera().translation_.x, cameraController_->GetCenter().x + 0.1f, 0.2f);
			cameraController_->GetCamera().rotation_.y = Lerp(cameraController_->GetCamera().rotation_.y, -0.1f, 0.1f);
			cameraController_->GetCamera().UpdateMatrix();

			if (cameraController_->GetCamera().translation_.x >= cameraController_->GetCenter().x && cameraController_->GetCamera().rotation_.y <= 0.0f)
			{
				cameraController_->GetCamera().translation_.x = cameraController_->GetCenter().x;
				cameraController_->GetCamera().rotation_.y = 0.0f;
				isFinisherEnd_ = false;
			}
		}
	}

	if (!isFinisherStart_ && !isFinisherEnd_)
	{
		//通常時のCameraControllerの更新
		cameraController_->Update(player_->GetWorldPosition(), enemy_->GetWorldPosition());
	}

	//勝ち負けの処理
	HandleGameOutcome();

	skyboxWorldTransform_.UpdateMatrixEuler();

	//操作説明の開閉
	if (input_->GetJoystickState() && !isKO_&& roundStartTimer_ <= 0 && migrationTimer == 200)
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && !isOpen_)
		{
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			isOpen_ = true;
			spriteCount_ = 1;
		}
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && isOpen_)
		{
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			isOpen_ = false;
			spriteCount_ = 0;
		}


		if (isOpen_)
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) || (input_->GetLeftStickX() > value_ && stickInputCooldown_ <= 0))
			{
				if (spriteCount_ < 3)
				{
					spriteCount_++;
					audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
					stickInputCooldown_ = 10;
				}
			}
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) || (input_->GetLeftStickX() < -value_ && stickInputCooldown_ <= 0))
			{
				if (spriteCount_ > 1)
				{
					spriteCount_--;
					audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
					stickInputCooldown_ = 10;
				}
			}

			//クールダウンを減らす
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

	hitStop_->Update();

	//InputLogの更新
	inputLog_->Update();

	//Camera、DebugCameraの処理
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

	if (isDebugCamera_)
	{
		camera_.matView_ = debugCamera_.GetCamera().matView_;
		camera_.matProjection_ = debugCamera_.GetCamera().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		camera_.UpdateMatrix();
	}
};

void GamePlayScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Skybox::PreDraw();

	//skybox_->Draw(skyboxWorldTransform_, cameraController_->GetCamera());

	Skybox::PostDraw();

	Model::PreDraw();

	////Skydomeの描画
	skydome_->Draw(cameraController_->GetCamera());

	if (!isOpen_)
	{
		//Game3dObjectManagerの描画
		game3dObjectManager_->Draw(cameraController_->GetCamera());

		//Enemyの弾の描画
		enemy_->BulletDraw(cameraController_->GetCamera());

		//BackGroundの描画
		backGround_->Draw(cameraController_->GetCamera());
	}

	Model::PostDraw();

	ParticleModel::PreDraw();

	if (GamePlayScene::roundStartTimer_ <= 0 && !isOpen_)
	{
		//Playerのparticle描画
		player_->ParticleDraw(cameraController_->GetCamera());

		//Enemyのparticle描画
		enemy_->ParticleDraw(cameraController_->GetCamera());
	}

	ParticleModel::PostDraw();

	Line::PreDraw();

	if (isDebug_ && !isOpen_)
	{
		//Playerの当たり判定描画
		player_->CollisionDraw(cameraController_->GetCamera());

		//Enemyの当たり判定描画
		enemy_->CollisionDraw(cameraController_->GetCamera());
	}

	Line::PostDraw();

	Model::BonePreDraw();

#ifdef _ADJUSTMENT

	////playerのbone描画
	//player_->BoneDraw(cameraController_->GetCamera());

	////enemyのbone描画
	//enemy_->BoneDraw(cameraController_->GetCamera());

#endif

	Model::BonePostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//ラウンド開始表示
	if (roundStartTimer_ <= maxRoundStartTime_ && roundStartTimer_ > halfRoundStartTime_)
	{
		if (round_ == 1)
		{
			roundSprite_[0]->Draw();
		}
		else if (round_ == 2)
		{
			roundSprite_[1]->Draw();
		}
		else if (round_ == 3)
		{
			roundSprite_[2]->Draw();
		}
	}

	if (roundStartTimer_ <= halfRoundStartTime_ && roundStartTimer_ > 0)
	{
		fightSprite_->Draw();
	}

	//ラウンド終了時の勝敗表示
	if (migrationTimer < outComeTime_ && migrationTimer > 20)
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

	if (roundStartTimer_ <= 0 && !isOpen_)
	{
		frameUISprite_->Draw();

		player_->SpriteDraw();

		enemy_->SpriteDraw();

		numberOnesSprite_->Draw();
		numberTensSprite_->Draw();

		if (PlayerWinCount_ >= 1)
		{
			roundGetSprite_[1]->Draw();
		}

		if (PlayerWinCount_ >= 2)
		{
			roundGetSprite_[0]->Draw();
		}


		if (EnemyWinCount_ >= 1)
		{
			roundGetSprite_[3]->Draw();
		}

		if (EnemyWinCount_ >= 2)
		{
			roundGetSprite_[2]->Draw();
		}

	}

	/*if (isOpen_)
	{
		commandListBackSprite_->Draw();
	}*/

	//操作説明
	if (isOpen_ && spriteCount_ == 1)
	{
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 2)
	{
		attackCommandListSprite_[0]->Draw();
	}

	if (isOpen_ && spriteCount_ == 3)
	{
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
	/*ImGui::Begin("PlayScene");
	ImGui::Text("roundTransitionTimer %d", roundTransitionTimer_);
	ImGui::Checkbox("isDebug_", &isDebug_);
	ImGui::End();*/

	/*player_->ImGui("Player");
	enemy_->ImGui("Enemy");*/

	//camera_.ImGui();

	//cameraController_->ImGui();
}

void GamePlayScene::UpdateNumberSprite()
{
	int tensDigit = currentSeconds_ / 10;
	int onesDigit = currentSeconds_ % 10;

	tensTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(tensDigit) + ".png");
	onesTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(onesDigit) + ".png");

	numberTensSprite_->SetTexture(tensTextureHandle_);
	numberOnesSprite_->SetTexture(onesTextureHandle_);
}

float GamePlayScene::Random(float min_value, float max_value)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min_value, max_value);

	return dis(gen);
}

//TODO:長いので簡潔にする
void GamePlayScene::HandleGameOutcome()
{
	//Playerが勝ったとき
	if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == 2 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == 2 && PlayerWinCount_ == 0 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0 && round_ == 3 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f && !player_->GetIsFinisherSecondAttack() && !player_->GetIsTackle())
		{
			PlayerWinCount_ = 2;
		}
	}

	//時間切れ
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 2 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 2 && PlayerWinCount_ == 0 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 3 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 2;
		}
	}

	if (PlayerWinCount_ == 2)
	{
		isTransitionStart_ = true;
	}

	//Enemyが勝ったとき
	if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < testKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == 2 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < testKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == 2 && EnemyWinCount_ == 0 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < testKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0 && round_ == 3 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < testKoActiveTime_ && player_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = 2;
		}
	}

	//時間切れ
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 2 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 2 && EnemyWinCount_ == 0 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 3 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			EnemyWinCount_ = 2;
		}
	}

	if (EnemyWinCount_ == 2)
	{
		if (isTransitionStart_ == false && isTransitionEnd_ == true)
		{
			isTransitionStart_ = true;
		}
	}

	//時間切れ(ドロー)
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isDrow_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isDrow_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && PlayerWinCount_ == 1 && EnemyWinCount_ == 0 && round_ == 2 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 2 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && PlayerWinCount_ == 1 && EnemyWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;
		isTimeOver_ = true;

		if (migrationTimer < testKoActiveTime_)
		{
			EnemyWinCount_ = 2;
		}
	}

	//相打ち
	if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isDrow_ = true;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isDrow_ = true;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && PlayerWinCount_ == 1 && EnemyWinCount_ == 0 && round_ == 2 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 2 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && PlayerWinCount_ == 1 && EnemyWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < testKoActiveTime_ && enemy_->GetWorldPosition().y <= 0.0f)
		{
			EnemyWinCount_ = 2;
		}
	}

	//トランジション
	if (!isTransitionEnd_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= 0.0f)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransitionStart_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= 1.0f)
		{
			if (PlayerWinCount_ == 2)
			{
				PostProcess::GetInstance()->SetIsGrayScaleActive(false);
				PostProcess::GetInstance()->SetIsVignetteActive(false);
				sceneManager_->ChangeScene("GameWinScene");
				return;
			}

			if (EnemyWinCount_ == 2)
			{
				PostProcess::GetInstance()->SetIsGrayScaleActive(false);
				PostProcess::GetInstance()->SetIsVignetteActive(false);
				sceneManager_->ChangeScene("GameLoseScene");
				return;
			}
		}
	}

	int roundCount = 1;

	if (isRoundTransition_)
	{
		if (isDrow_)
		{
			roundCount = 3;
		}
		else if (round_ == 1)
		{
			roundCount = 2;
		}
		else if (round_ == 2)
		{
			roundCount = 3;
		}
		else if (round_ == 3)
		{
			roundCount = 3;
		}

		RoundTransition(roundCount);
	}
}

void GamePlayScene::RoundTransition(int round)
{
	if (isRoundTransition_)
	{
		roundTransitionTimer_--;

		if (roundTransitionTimer_ > 75)
		{
			transitionColor_.w = Lerp(transitionColor_.w, 1.0f, 0.1f);
			transitionSprite_->SetColor(transitionColor_);
		}
		else if (roundTransitionTimer_ <= 65 && roundTransitionTimer_ > 0)
		{
			transitionColor_.w = Lerp(transitionColor_.w, 0.0f, 0.1f);
			transitionSprite_->SetColor(transitionColor_);

		}
		else if (roundTransitionTimer_ <= 0)
		{
			isRoundTransition_ = false;
			roundTransitionTimer_ = 150;
		}

		if (roundTransitionTimer_ == 75)
		{
			isPlayerWin_ = false;
			isDrow_ = false;
			isTimeOver_ = false;
			round_ = round;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100;
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
		}
	}
}