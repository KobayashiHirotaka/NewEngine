#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Application/GameObject/Character/ICharacter.h"
#include <cassert>

int GamePlayScene::migrationTimer = 200;
float GamePlayScene::roundStartTimer_ = 100.0f;

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

	//Playerの生成、初期化
	player_ = game3dObjectManager_->GetGameObject<Player>("Player");

	//Enemyの生成、初期化
	enemy_ = game3dObjectManager_->GetGameObject<Enemy>("Enemy");

	player_->SetEnemy(enemy_);
	enemy_->SetPlayer(player_);

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

	//基本操作説明用のSprite
	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayGeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明用のSprite
	attackCommandListTextureHandle_[0] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], { 0.0f,0.0f }));

	attackCommandListTextureHandle_[1] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList2.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));

	commandListBackTextureHandle_ = TextureManager::LoadTexture("resource/images/CommandListBack.png");
	commandListBackSprite_.reset(Sprite::Create(commandListBackTextureHandle_, { 0.0f,0.0f }));

	roundTextureHandle_[0] = TextureManager::LoadTexture("resource/images/Round1.png");
	roundTextureHandle_[1] = TextureManager::LoadTexture("resource/images/Round2.png");
	roundTextureHandle_[2] = TextureManager::LoadTexture("resource/images/Round3.png");

	roundSprite_[0].reset(Sprite::Create(roundTextureHandle_[0], { 0.0f, 0.0f }));
	roundSprite_[1].reset(Sprite::Create(roundTextureHandle_[1], { 0.0f, 0.0f }));
	roundSprite_[2].reset(Sprite::Create(roundTextureHandle_[2], { 0.0f, 0.0f }));

	fightTextureHandle_ = TextureManager::LoadTexture("resource/images/FIGHT.png");

	fightSprite_.reset(Sprite::Create(fightTextureHandle_, { 0.0f, 0.0f }));

	roundGetTextureHandle_ = TextureManager::LoadTexture("resource/images/RoundGet.png");

	roundGetSprite_[0].reset(Sprite::Create(roundGetTextureHandle_, { 400.0f, 70.0f }));
	roundGetSprite_[1].reset(Sprite::Create(roundGetTextureHandle_, { 480.0f, 70.0f }));
	roundGetSprite_[2].reset(Sprite::Create(roundGetTextureHandle_, { 800.0f, 70.0f }));
	roundGetSprite_[3].reset(Sprite::Create(roundGetTextureHandle_, { 720.0f, 70.0f }));

	winTextureHandle_ = TextureManager::LoadTexture("resource/images/WIN.png");
	loseTextureHandle_ = TextureManager::LoadTexture("resource/images/LOSE.png");
	drowTextureHandle_ = TextureManager::LoadTexture("resource/images/Drow.png");

	winSprite_.reset(Sprite::Create(winTextureHandle_, { 0.0f, 0.0f }));
	loseSprite_.reset(Sprite::Create(loseTextureHandle_, { 0.0f, 0.0f }));
	drowSprite_.reset(Sprite::Create(drowTextureHandle_, { 0.0f, 0.0f }));

	frameUITextureHandle_ = TextureManager::LoadTexture("resource/images/frameUI.png");
	frameUISprite_.reset(Sprite::Create(frameUITextureHandle_, { 0.0f, 0.0f }));

	tensTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");
	onesTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");

	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, { 580.0f, 0.0f }));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, { 620.0f, 0.0f }));

	playerIconTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayerIcon.png");
	enemyIconTextureHandle_ = TextureManager::LoadTexture("resource/images/EnemyIcon.png");

	playerIconSprite_.reset(Sprite::Create(playerIconTextureHandle_, { 60.0f, 20.0f }));
	playerIconSprite_->SetSize({ 120.0f,120.0f });

	enemyIconSprite_.reset(Sprite::Create(enemyIconTextureHandle_, { 1100.0f, 20.0f }));
	enemyIconSprite_->SetSize({ 120.0f,120.0f });

	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");

	//ラウンドごとの時間
	currentSeconds_ = 99;
	UpdateNumberSprite();

	migrationTimer = 200;

	frameTime = 1.0f / 60.0f;
	elapsedTime = 0.0f;

	roundStartTimer_ = 100.0f;

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
			elapsedTime += frameTime;

			//タイムカウントを更新
			if (currentSeconds_ > 0 && elapsedTime >= 1.0f && migrationTimer == 200)
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

		//Game3dObjectManagerの更新
		game3dObjectManager_->Update();

		//Skydomeの更新
		skydome_->Update();

		//BackGroundの更新
		backGround_->Update();

		//シェイク
		if ((player_->GetIsShake() || enemy_->GetIsShake()) && !isPlayerWin_ && roundStartTimer_ <= 0)
		{
			isShake_ = true;
			shakeTimer_ = kShakeTime;
		}

		if (isShake_)
		{
			camera_.translation_.y = Random(shakePower_.x, shakePower_.y);

			if (--shakeTimer_ < 0)
			{
				isShake_ = false;
				camera_.translation_.y = 1.0f;
			}
		}
	}

	//PostEffectの値変更
	if (player_->GetHP() >= -25.0f)
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}
	else
	{
		PostProcess::GetInstance()->SetIsVignetteActive(false);
	}

	//右向き用のカメラ移動
	if (player_->GetIsFinisher() && player_->GetFinisherTimer() != 120)
	{
		if (player_->GetIsDirectionRight())
		{
			camera_.translation_ = Lerp(camera_.translation_, { player_->GetWorldPosition().x + 4.0f,
			player_->GetWorldPosition().y + 1.0f, player_->GetWorldPosition().z - 4.5f }, 0.1f);

			camera_.rotation_.y = Lerp(camera_.rotation_.y, -0.7f, 0.1f);
		}
		else
		{
			camera_.translation_ = Lerp(camera_.translation_, { player_->GetWorldPosition().x - 4.0f,
			player_->GetWorldPosition().y + 1.0f, player_->GetWorldPosition().z - 4.5f }, 0.1f);

			camera_.rotation_.y = Lerp(camera_.rotation_.y, 0.7f, 0.1f);
		}
	}
	else
	{
		Vector3 translation_ = { 0.0f,1.0f,-13.0f };

		//camera_.translation_ = translation_;
		//camera_.rotation_.y = 0.0f;
		camera_.translation_ = Lerp(camera_.translation_, translation_, 0.1f);
		camera_.rotation_.y = Lerp(camera_.rotation_.y, 0.0f, 0.1f);
	}

	//勝ち負けの処理
	HandleGameOutcome();

	skyboxWorldTransform_.UpdateMatrixEuler();

	//操作説明の開閉
	if (input_->GetJoystickState())
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
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT))
			{
				if (spriteCount_ < 3)
				{
					spriteCount_++;
					audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
				}
			}
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				if (spriteCount_ > 1)
				{
					spriteCount_--;
					audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
				}
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

	//Camera、DebugCameraの処理
	debugCamera_.Update();

#ifdef _DEBUG

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

#endif // DEBUG

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

	//skybox_->Draw(skyboxWorldTransform_, camera_);

	Skybox::PostDraw();

	Model::PreDraw();

	////Skydomeの描画
	skydome_->Draw(camera_);

	if (!isOpen_)
	{
		//Game3dObjectManagerの描画
		game3dObjectManager_->Draw(camera_);

		//Enemyの弾の描画
		enemy_->BulletDraw(camera_);

		//BackGroundの描画
		backGround_->Draw(camera_);
	}

	Model::PostDraw();

	ParticleModel::PreDraw();

	if (GamePlayScene::roundStartTimer_ <= 0 && !isOpen_)
	{
		//Playerのparticle描画
		player_->ParticleDraw(camera_);

		//Enemyのparticle描画
		enemy_->ParticleDraw(camera_);
	}

	ParticleModel::PostDraw();

	Model::BonePreDraw();

	////playerのbone描画
	//player_->BoneDraw(camera_);

	////enemyのbone描画
	//enemy_->BoneDraw(camera_);

	Model::BonePostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//ラウンド開始表示
	if (roundStartTimer_ <= 100 && roundStartTimer_ > 50 && round_ == 1)
	{
		roundSprite_[0]->Draw();
	}

	if (roundStartTimer_ <= 100 && roundStartTimer_ > 50 && round_ == 2)
	{
		roundSprite_[1]->Draw();
	}

	if (roundStartTimer_ <= 100 && roundStartTimer_ > 50 && round_ == 3)
	{
		roundSprite_[2]->Draw();
	}

	if (roundStartTimer_ <= 50 && roundStartTimer_ > 0)
	{
		fightSprite_->Draw();
	}

	//ラウンド終了時の勝敗表示
	if (migrationTimer < 150 && migrationTimer > 0)
	{
		if (isPlayerWin_)
		{
			winSprite_->Draw();
		}

		if (!isPlayerWin_ && !isDrow_)
		{
			loseSprite_->Draw();
		}

		if (isDrow_)
		{
			drowSprite_->Draw();
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

	playerIconSprite_->Draw();

	enemyIconSprite_->Draw();
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	transitionSprite_->Draw();

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{
	
}

void GamePlayScene::ImGui()
{
	ImGui::Begin("PlayScene");
	ImGui::Text("roundTransitionTimer %d", roundTransitionTimer_);
	ImGui::End();

	player_->ImGui("Player");
	enemy_->ImGui("Enemy");
	backGround_->ImGui();

	camera_.ImGui();
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

void GamePlayScene::HandleGameOutcome()
{
	//Playerが勝ったとき
	if (enemy_->GetHP() <= 0 && player_->GetHP() < 0.0f && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0.0f && round_ == 2 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0.0f && round_ == 2 && PlayerWinCount_ == 0 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() < 0.0f && round_ == 3 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}

	//時間切れ
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 2 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 2 && PlayerWinCount_ == 0 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 3 && PlayerWinCount_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}

	if (PlayerWinCount_ == 2)
	{
		isTransitionStart_ = true;
	}

	//Enemyが勝ったとき
	if (player_->GetHP() >= 0 && enemy_->GetHP() > 0.0f && round_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0.0f && round_ == 2 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0.0f && round_ == 2 && EnemyWinCount_ == 0 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (player_->GetHP() >= 0 && enemy_->GetHP() > 0.0f && round_ == 3 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}

	//時間切れ
	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 2 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 2 && EnemyWinCount_ == 0 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 1;
			isRoundTransition_ = true;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 3 && EnemyWinCount_ == 1 && !isRoundTransition_)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}

	if (EnemyWinCount_ == 2)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);

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

		if (migrationTimer < 0)
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

		if (migrationTimer < 0)
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

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 2 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && PlayerWinCount_ == 1 && EnemyWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}

	//相打ち
	if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && round_ == 1 && !isRoundTransition_)
	{
		migrationTimer--;
		isDrow_ = true;

		if (migrationTimer < 0)
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

		if (migrationTimer < 0)
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

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 2 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && PlayerWinCount_ == 1 && EnemyWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && player_->GetHP() >= 0 && EnemyWinCount_ == 1 && PlayerWinCount_ == 0 && round_ == 3 && !isRoundTransition_)
	{
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
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
		else if (roundTransitionTimer_ <= 75 && roundTransitionTimer_ > 0)
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
			round_ = round;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			PostProcess::GetInstance()->SetIsVignetteActive(false);
		}
	}
}