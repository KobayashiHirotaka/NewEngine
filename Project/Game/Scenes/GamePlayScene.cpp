#include "GamePlayScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameWinScene.h"
#include "GameLoseScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize(SceneManager* sceneManager)
{
	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	collisionManager_ = std::make_unique<CollisionManager>();

	UICommandListTextureHandle_ = TextureManager::LoadTexture("resource/UICommandList.png");
	UICommandListSprite_.reset(Sprite::Create(UICommandListTextureHandle_, { 0.0f,0.0f }));

	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/PlayGeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	attackCommandListTextureHandle_ = TextureManager::LoadTexture("resource/PlayAttackCommandList.png");
	attackCommandListSprite_.reset(Sprite::Create(attackCommandListTextureHandle_, { 0.0f,0.0f }));

	ground_.reset(Model::CreateFromOBJ("resource/Ground", "Ground.obj"));

	roundTextureHandle_[0] = TextureManager::LoadTexture("resource/Round1.png");
	roundTextureHandle_[1] = TextureManager::LoadTexture("resource/Round2.png");
	roundTextureHandle_[2] = TextureManager::LoadTexture("resource/Round3.png");

	roundSprite_[0].reset(Sprite::Create(roundTextureHandle_[0], {0.0f, 0.0f}));
	roundSprite_[1].reset(Sprite::Create(roundTextureHandle_[1], { 0.0f, 0.0f }));
	roundSprite_[2].reset(Sprite::Create(roundTextureHandle_[2], { 0.0f, 0.0f }));

	fightTextureHandle_ = TextureManager::LoadTexture("resource/FIGHT.png");

	fightSprite_.reset(Sprite::Create(fightTextureHandle_, { 0.0f, 0.0f }));

	roundGetTextureHandle_ = TextureManager::LoadTexture("resource/RoundGet.png");

	roundGetSprite_[0].reset(Sprite::Create(roundGetTextureHandle_, { 400.0f, 30.0f }));
	roundGetSprite_[1].reset(Sprite::Create(roundGetTextureHandle_, { 480.0f, 30.0f }));
	roundGetSprite_[2].reset(Sprite::Create(roundGetTextureHandle_, { 800.0f, 30.0f }));
	roundGetSprite_[3].reset(Sprite::Create(roundGetTextureHandle_, { 720.0f, 30.0f }));

	winTextureHandle_ = TextureManager::LoadTexture("resource/WIN.png");
	loseTextureHandle_ = TextureManager::LoadTexture("resource/LOSE.png");
	drowTextureHandle_= TextureManager::LoadTexture("resource/Drow.png");

	winSprite_.reset(Sprite::Create(winTextureHandle_, { 0.0f, 0.0f }));
	loseSprite_.reset(Sprite::Create(loseTextureHandle_, { 0.0f, 0.0f }));
	drowSprite_.reset(Sprite::Create(drowTextureHandle_, { 0.0f, 0.0f }));

	worldTransform_.Initialize();
	worldTransform_.translation = { 0.0f,-1.0f,0.0f };

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();
	
	player_ = std::make_unique<Player>();
	player_->Initialize();

	player_->SetEnemy(enemy_.get());
	enemy_->SetPlayer(player_.get());

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsVignetteActive(true);

	camera_.UpdateMatrix();

	currentSeconds_ = 99;
	UpdateNumberSprite();

	migrationTimer_ = 200;

	frameTime = 1.0f / 60.0f; 
	elapsedTime = 0.0f;

	roundStartTimer_ = 100.0f;

	isPlayerWin_ = false;
	isDrow_ = false;

	selectSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Select.wav");
};

void GamePlayScene::Update(SceneManager* sceneManager)
{
	roundStartTimer_--;

	if (migrationTimer_ >= 150 && roundStartTimer_ <= 0 && !isOpen_)
	{
		player_->Update();

		enemy_->Update();

		// 時間経過を加算
		elapsedTime += frameTime;

		// タイムカウントを更新
		if (currentSeconds_ > 0 && elapsedTime >= 1.0f) {
			currentSeconds_--;
			UpdateNumberSprite();

			// elapsedTimeをリセット
			elapsedTime = 0.0f;
		}
	}


	skydome_->Update();

	//Playerが勝ったとき
	if (enemy_->GetHP() <= 0 && round_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = true;

		if (migrationTimer_ < 0)
		{
			round_ = 2;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 2 && PlayerWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = true;
		
		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 2 && PlayerWinCount_ == 0)
	{
		migrationTimer_--;
		isPlayerWin_ = true;
		
		if (migrationTimer_ < 0)
		{
			round_ = 3;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 3 && PlayerWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = true;
		
		if (migrationTimer_ < 0)
		{
			migrationTimer_ = 60;
			Initialize(sceneManager);
			PlayerWinCount_ = 2;
		}
	}

	if (currentSeconds_ <= 0 && enemy_->GetHP() < player_->GetHP() && round_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = true;

		if (migrationTimer_ < 0)
		{
			round_ = 2;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() < player_->GetHP() && round_ == 2 && PlayerWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = true;
		
		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			PlayerWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() < player_->GetHP() && round_ == 2 && PlayerWinCount_ == 0)
	{
		migrationTimer_--;
		isPlayerWin_ = true;

		if (migrationTimer_ < 0)
		{
			round_ = 3;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() < player_->GetHP() && round_ == 3 && PlayerWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = true;

		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			PlayerWinCount_ = 2;
		}
	}

	if (PlayerWinCount_ == 2)
	{
		sceneManager->ChangeScene(new GameWinScene);
	}

	//Enemyが勝ったとき
	if (player_->GetHP() <= 0 && round_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
		
		if (migrationTimer_ < 0)
		{
			round_ = 2;
			Initialize(sceneManager);
			EnemyWinCount_ = 1;
		}
	}
	else if (player_->GetHP() <= 0 && round_ == 2 && EnemyWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
		
		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			EnemyWinCount_ = 2;
		}
	}
	else if (player_->GetHP() <= 0 && round_ == 2 && EnemyWinCount_ == 0)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
	
		if (migrationTimer_ < 0)
		{
			round_ = 3;
			Initialize(sceneManager);
			EnemyWinCount_ = 1;
		}
	}
	else if (player_->GetHP() <= 0 && round_ == 3 && EnemyWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
		
		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			EnemyWinCount_ = 2;
		}
	}

	if (currentSeconds_ <= 0 && enemy_->GetHP() > player_->GetHP() && round_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
	
		if (migrationTimer_ < 0)
		{
			round_ = 2;
			Initialize(sceneManager);
			EnemyWinCount_ = 1;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() > player_->GetHP() && round_ == 2 && EnemyWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
		
		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			EnemyWinCount_ = 2;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() > player_->GetHP() && round_ == 2 && EnemyWinCount_ == 0)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
		
		if (migrationTimer_ < 0)
		{
			round_ = 3;
			Initialize(sceneManager);
			EnemyWinCount_ = 1;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() > player_->GetHP() && round_ == 3 && EnemyWinCount_ == 1)
	{
		migrationTimer_--;
		isPlayerWin_ = false;
		
		if (migrationTimer_ < 0)
		{
			Initialize(sceneManager);
			EnemyWinCount_ = 2;
		}
	}

	if (EnemyWinCount_ == 2)
	{
		sceneManager->ChangeScene(new GameLoseScene);
	}

	if (currentSeconds_ <= 0 && enemy_->GetHP() == player_->GetHP() && round_ == 1)
	{
		migrationTimer_--;
		isDrow_ = true;

		if (migrationTimer_ < 0)
		{
			round_ = 3;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;
		}
	}
	else if (currentSeconds_ <= 0 && enemy_->GetHP() == player_->GetHP() && round_ == 3)
	{
		migrationTimer_--;
		isDrow_ = true;
		
		if (migrationTimer_ < 0)
		{
			round_ = 3;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
			EnemyWinCount_ = 1;
		}
	}

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !isOpen_ && roundStartTimer_ < 0)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			isOpen_ = true;
			spriteCount_ = 1;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isOpen_)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			isOpen_ = false;
			spriteCount_ = 0;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) && isOpen_ && spriteCount_ == 1)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			spriteCount_ = 2;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) && isOpen_ && spriteCount_ == 2)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			spriteCount_ = 1;
		}
	}

	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_.get());

	if (player_->GetPlayerWeapon()->GetIsAttack())
	{
		collisionManager_->AddCollider(player_->GetPlayerWeapon());
	}

	if (enemy_->GetEnemyWeapon()->GetIsAttack())
	{
		collisionManager_->AddCollider(enemy_->GetEnemyWeapon());
	}

	collisionManager_->AddCollider(enemy_.get());
	
	collisionManager_->CheckAllCollision();

	worldTransform_.UpdateMatrix();

	camera_.UpdateMatrix();
};

void GamePlayScene::Draw(SceneManager* sceneManager)
{
	Model::PreDraw();

	if (!isOpen_)
	{
		player_->Draw(camera_);

		enemy_->Draw(camera_);
	}

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	if (!isOpen_)
	{
		ground_->Draw(worldTransform_, camera_);
	}

	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	player_->DrawParticle(camera_);

	enemy_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	if (migrationTimer_ < 150)
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
		player_->DrawSprite();

		enemy_->DrawSprite();

		numberOnesSprite_->Draw();
		numberTensSprite_->Draw();

		if (PlayerWinCount_ == 1)
		{
			roundGetSprite_[1]->Draw();
		}

		if (PlayerWinCount_ == 2)
		{
			roundGetSprite_[0]->Draw();
		}


		if (EnemyWinCount_ == 1)
		{
			roundGetSprite_[3]->Draw();
		}

		if (EnemyWinCount_ == 2)
		{
			roundGetSprite_[2]->Draw();
		}

	}
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	if (roundStartTimer_ <= 0 && !isOpen_)
	{
		UICommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 1)
	{
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 2)
	{
		attackCommandListSprite_->Draw();
	}

	Sprite::PostDraw();
};

void GamePlayScene::UpdateNumberSprite()
{
	int tensDigit = currentSeconds_ / 10;
	int onesDigit = currentSeconds_ % 10;

	tensTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(tensDigit) + ".png");
	onesTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(onesDigit) + ".png");

	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, { 580.0f, 0.0f }));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, { 620.0f, 0.0f }));
}