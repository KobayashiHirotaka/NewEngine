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

	collisionManager_ = std::make_unique<CollisionManager>();

	winTextureHandle_ = TextureManager::Load("resource/WIN.png");
	loseTextureHandle_ = TextureManager::Load("resource/LOSE.png");
	drowTextureHandle_= TextureManager::Load("resource/Drow.png");

	winSprite_.reset(Sprite::Create(winTextureHandle_, { 0.0f, 0.0f }));
	loseSprite_.reset(Sprite::Create(loseTextureHandle_, { 0.0f, 0.0f }));
	drowSprite_.reset(Sprite::Create(drowTextureHandle_, { 0.0f, 0.0f }));

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

	frameTime = 1.0f / 60.0f;  // 60FPSを仮定
	elapsedTime = 0.0f;

	isPlayerWin_ = false;
	isDrow_ = false;
};

void GamePlayScene::Update(SceneManager* sceneManager)
{
	if (migrationTimer_ >= 150)
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
			PlayerWinCount_ = 1;
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

	camera_.UpdateMatrix();
};

void GamePlayScene::Draw(SceneManager* sceneManager)
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	player_->Draw(camera_);

	enemy_->Draw(camera_);

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

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	player_->DrawSprite();

	enemy_->DrawSprite();

	numberOnesSprite_->Draw();
	numberTensSprite_->Draw();

	Sprite::PostDraw();
};

void GamePlayScene::UpdateNumberSprite()
{
	int tensDigit = currentSeconds_ / 10;
	int onesDigit = currentSeconds_ % 10;

	tensTextureHandle_ = TextureManager::Load("resource/number/" + std::to_string(tensDigit) + ".png");
	onesTextureHandle_ = TextureManager::Load("resource/number/" + std::to_string(onesDigit) + ".png");

	// 10の位の数字スプライトの位置は左に少しずらす例
	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, { 580.0f, 0.0f }));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, { 620.0f, 0.0f }));
}