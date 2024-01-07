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
};

void GamePlayScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Play");
	ImGui::Text("1Key : PostProcess ON");
	ImGui::Text("2Key : Bloom ON");
	ImGui::Text("3Key : Vignette ON");
	ImGui::End();

	player_->Update();

	enemy_->Update();

	skydome_->Update();

	//勝ち負け表示の処理
	if (input_->PushKey(DIK_P))
	{
		player_->SetHP(0);
	}

	if (input_->PushKey(DIK_O))
	{
		enemy_->SetHP(0);
	}

	//Playerが勝ったとき
	if (enemy_->GetHP() <= 0 && round_ == 1)
	{
		migrationTimer_--;
		ImGui::Begin("PlayerWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			round_ = 2;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 2 && PlayerWinCount_ == 1)
	{
		migrationTimer_--;
		ImGui::Begin("PlayerWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			Initialize(sceneManager);
			PlayerWinCount_ = 2;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 2 && PlayerWinCount_ == 0)
	{
		migrationTimer_--;
		ImGui::Begin("PlayerWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			round_ = 3;
			Initialize(sceneManager);
			PlayerWinCount_ = 1;
		}
	}
	else if (enemy_->GetHP() <= 0 && round_ == 3 && PlayerWinCount_ == 1)
	{
		migrationTimer_--;
		ImGui::Begin("PlayerWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
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
		ImGui::Begin("EnemyWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			round_ = 2;
			Initialize(sceneManager);
			EnemyWinCount_ = 1;
		}
	}
	else if (player_->GetHP() <= 0 && round_ == 2 && EnemyWinCount_ == 1)
	{
		migrationTimer_--;
		ImGui::Begin("EnemyWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			Initialize(sceneManager);
			EnemyWinCount_ = 2;
		}
	}
	else if (player_->GetHP() <= 0 && round_ == 2 && EnemyWinCount_ == 0)
	{
		migrationTimer_--;
		ImGui::Begin("EnemyWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			round_ = 3;
			Initialize(sceneManager);
			EnemyWinCount_ = 1;
		}
	}
	else if (player_->GetHP() <= 0 && round_ == 3 && EnemyWinCount_ == 1)
	{
		migrationTimer_--;
		ImGui::Begin("EnemyWin");
		ImGui::End();

		if (migrationTimer_ < 0 && enemy_->GetIsDown() == false)
		{
			migrationTimer_ = 60;
			Initialize(sceneManager);
			EnemyWinCount_ = 2;
		}
	}

	if (EnemyWinCount_ == 2)
	{
		sceneManager->ChangeScene(new GameLoseScene);
	}

	ImGui::Begin("round");
	ImGui::Text("round %d", round_);
	ImGui::Text("PlayerWinCount %d", PlayerWinCount_);
	ImGui::Text("EnemyWinCount %d", EnemyWinCount_);
	ImGui::End();

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

	/*if (input_->PushKey(DIK_SPACE))
	{
		sceneManager->ChangeScene(new GameEndScene);
	}*/

	camera_.UpdateMatrix();
};

void GamePlayScene::Draw(SceneManager* sceneManager)
{

	DirectXCore::GetInstance()->ClearDepthBuffer();

	Model::PreDraw();

	player_->Draw(camera_);

	enemy_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	player_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	player_->DrawSprite();

	Sprite::PostDraw();
};