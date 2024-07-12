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
	//textureManagerのinstance
	textureManager_ = TextureManager::GetInstance();

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//audioのinstance
	audio_ = Audio::GetInstance();

	//collisionManagerのinstance
	collisionManager_ = std::make_unique<CollisionManager>();

	//game3dObjectManagerのinstance
	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->Initialize();

	//postProcessのinstance
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//postEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);
	PostProcess::GetInstance()->SetIsLuminanceBasedOutlineActive(true);
	//PostProcess::GetInstance()->SetIsDepthBasedOutlineActive(true);

	//Levelの読み込み
	levelLoarder_ = LevelLoader::GetInstance();
	levelLoarder_->LoadLevel("LevelData");

	//modelの読み込み
	//modelManager_->LoadModel("resource/skydome", "skydome.obj");

	//playerの生成、初期化
	player_ = game3dObjectManager_->GetGameObject<Player>("Player");

	//enemyの生成、初期化
	enemy_ = game3dObjectManager_->GetGameObject<Enemy>("Enemy");

	player_->SetEnemy(enemy_);
	enemy_->SetPlayer(player_);

	//skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//リソースの初期化(sprite,se)
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

	// テクスチャは仮の初期値を設定
	tensTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");
	onesTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");

	// スプライトの生成
	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, { 580.0f, 0.0f }));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, { 620.0f, 0.0f }));

	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	currentSeconds_ = 99;
	UpdateNumberSprite();

	migrationTimer = 200;

	frameTime = 1.0f / 60.0f;
	elapsedTime = 0.0f;

	roundStartTimer_ = 100.0f;

	isPlayerWin_ = false;
	isDrow_ = false;

	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");

	//debugCameraの初期化
	debugCamera_.Initialize();
};

void GamePlayScene::Update()
{
	//テスト用の処理
	if (input_->PushKey(DIK_RETURN))
	{
		player_->SetIsReset(true);
		enemy_->SetIsReset(true);
	}
	//テスト用の処理(ここまで)

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

	if (!player_->GetIsFinisherEffect())
	{
		//player,enemyの更新
		game3dObjectManager_->Update();
	}

	//skydomeの更新
	skydome_->Update();

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

	//勝ち負けの処理
	HandleGameOutcome();

	if (player_->GetHP() >= -25.0f)
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}
	else
	{
		PostProcess::GetInstance()->SetIsVignetteActive(false);
	}

	//当たり判定
	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_);

	collisionManager_->AddCollider(enemy_);

	collisionManager_->CheckAllCollision();

	//シーン切り替え
	if (input_->PushKey(DIK_N))
	{
		sceneManager_->ChangeScene("GameWinScene");
		return;
	}

	if (input_->PushKey(DIK_M))
	{
		sceneManager_->ChangeScene("GameLoseScene");
		return;
	}

	//camera、debugCameraの処理
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

	camera_.ImGui();

	//imGui
	ImGui::Begin("PlayScene");
	ImGui::Text("MKey : WinScene");
	ImGui::Text("NKey : WinScene");
	ImGui::End();
};

void GamePlayScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//player,enemyの描画
	game3dObjectManager_->Draw(camera_);

	//skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	if (GamePlayScene::roundStartTimer_ <= 0)
	{
		//playerのparticle描画
		player_->DrawParticle(camera_);

		//enemyのparticle描画
		enemy_->DrawParticle(camera_);
	}

	ParticleModel::PostDraw();

	Model::BonePreDraw();

	////playerのbone描画
	//player_->BoneDraw(camera_);

	////enemyのbone描画
	//enemy_->BoneDraw(camera_);

	Model::BonePostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

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

	if (roundStartTimer_ <= 0 && !isOpen_)
	{
		frameUISprite_->Draw();

		player_->DrawSprite();

		enemy_->DrawSprite();

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

	/*if (roundStartTimer_ <= 0 && !isOpen_)
	{
		UICommandListSprite_->Draw();
	}*/

	if (isOpen_ && spriteCount_ == 1)
	{
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 2)
	{
		attackCommandListSprite_->Draw();
	}

	transitionSprite_->Draw();

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{
	
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