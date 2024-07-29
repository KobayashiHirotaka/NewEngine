#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
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

	//PostProcessのinstance
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//PostEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);
	PostProcess::GetInstance()->SetIsLuminanceBasedOutlineActive(true);
	PostProcess::GetInstance()->SetIsHSVFilterActive(true);

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

	//BackGroundの生成、初期化
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize();

	//Skyboxの生成、初期化
	skybox_ = std::make_unique<Skybox>();
	skybox_->Create();

	//SkyboxのWorldTransformの初期化
	skyboxWorldTransform_.Initialize();

	//リソース
	UICommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/UICommandList.png");
	UICommandListSprite_.reset(Sprite::Create(UICommandListTextureHandle_, { 0.0f,0.0f }));

	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayGeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	attackCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayAttackCommandList.png");
	attackCommandListSprite_.reset(Sprite::Create(attackCommandListTextureHandle_, { 0.0f,0.0f }));

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
	//ラウンド間の時間の処理
	roundStartTimer_--;

	if (roundStartTimer_ <= 0 && !isOpen_)
	{
		//時間経過を加算
		elapsedTime += frameTime;

		//タイムカウントを更新
		if (currentSeconds_ > 0 && elapsedTime >= 1.0f) {
			currentSeconds_--;
			UpdateNumberSprite();

			//elapsedTimeをリセット
			elapsedTime = 0.0f;
		}
	}

	//Game3dObjectManagerの更新
	game3dObjectManager_->Update();

	//Skydomeの更新
	skydome_->Update();

	//BackGroundの更新
	backGround_->Update();

	//SkyboxのWorldTransformの更新
	skyboxWorldTransform_.UpdateMatrixEuler();

	//シェイク
	if (player_->GetIsShake() || enemy_->GetIsShake() && !isPlayerWin_ && roundStartTimer_ <= 0)
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

	//PostEffectの値変更
	if (player_->GetHP() >= -25.0f)
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}
	else
	{
		PostProcess::GetInstance()->SetIsVignetteActive(false);
	}

	//勝ち負けの処理
	HandleGameOutcome();

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

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

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

	Skybox::PreDraw();

	//Skyboxの描画
	skybox_->Draw(skyboxWorldTransform_, camera_);

	Skybox::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Game3dObjectManagerの描画
	game3dObjectManager_->Draw(camera_);

	//Enemyの弾の描画
	enemy_->BulletDraw(camera_);

	//Skydomeの描画
	skydome_->Draw(camera_);

	//BackGroundの描画
	backGround_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	if (GamePlayScene::roundStartTimer_ <= 0)
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
	if (migrationTimer < 150)
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
	
	ImGui::End();

	player_->ImGui("Player");
	enemy_->ImGui("Enemy");

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
	if (enemy_->GetHP() <= 0 && round_ == 1)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			round_ = 2;
			PlayerWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 2 && PlayerWinCount_ == 1)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 2 && PlayerWinCount_ == 0)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			round_ = 3;
			PlayerWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 3 && PlayerWinCount_ == 1)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}

	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 1)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			round_ = 2;
			PlayerWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 2 && PlayerWinCount_ == 1)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 2 && PlayerWinCount_ == 0)
	{
		migrationTimer--;
		isPlayerWin_ = true;

		if (migrationTimer < 0)
		{
			round_ = 3;
			PlayerWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) < abs(player_->GetHP()) && round_ == 3 && PlayerWinCount_ == 1)
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
	if (player_->GetHP() >= 0 && round_ == 1)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			round_ = 2;
			EnemyWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (player_->GetHP() >= 0 && round_ == 2 && EnemyWinCount_ == 1)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (player_->GetHP() >= 0 && round_ == 2 && EnemyWinCount_ == 0)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			round_ = 3;
			EnemyWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (player_->GetHP() >= 0 && round_ == 3 && EnemyWinCount_ == 1)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}

	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 1)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			round_ = 2;
			EnemyWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 2 && EnemyWinCount_ == 1)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			EnemyWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 2 && EnemyWinCount_ == 0)
	{
		PostProcess::GetInstance()->SetIsGrayScaleActive(true);
		migrationTimer--;
		isPlayerWin_ = false;

		if (migrationTimer < 0)
		{
			PostProcess::GetInstance()->SetIsGrayScaleActive(false);
			round_ = 3;
			EnemyWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) > abs(player_->GetHP()) && round_ == 3 && EnemyWinCount_ == 1)
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

	if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && round_ == 1)
	{
		migrationTimer--;
		isDrow_ = true;

		if (migrationTimer < 0)
		{
			round_ = 3;
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
		}
	}
	else if (currentSeconds_ <= 0 && abs(enemy_->GetHP()) == abs(player_->GetHP()) && round_ == 3)
	{
		migrationTimer--;
		isDrow_ = true;

		if (migrationTimer < 0)
		{
			round_ = 3;
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;

			player_->Reset();
			enemy_->Reset();

			currentSeconds_ = 99;
			UpdateNumberSprite();

			migrationTimer = 200;

			frameTime = 1.0f / 60.0f;
			elapsedTime = 0.0f;

			roundStartTimer_ = 100.0f;

			isPlayerWin_ = false;
			isDrow_ = false;
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
				sceneManager_->ChangeScene("GameWinScene");
				return;
			}

			if (EnemyWinCount_ == 2)
			{
				PostProcess::GetInstance()->SetIsGrayScaleActive(false);
				sceneManager_->ChangeScene("GameLoseScene");
				return;
			}
		}
	}
}