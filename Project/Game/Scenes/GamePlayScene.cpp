#include "GamePlayScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameEndScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize(SceneManager* sceneManager)
{
	input_ = Input::GetInstance();

	collisionManager_ = std::make_unique<CollisionManager>();

	camera_.Initialize();

	enemyWeaponModel_.reset(Model::CreateFromOBJ("resource/hammer", "hammer.obj"));

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/float_Body", "float_Body.obj"));
	modelFighterHead_.reset(Model::CreateFromOBJ("resource/float_Head", "float_Head.obj"));
	modelFighterPHead_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("resource/float_L_arm", "float_L_arm.obj"));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("resource/float_R_arm", "float_R_arm.obj"));

	std::vector<Model*> enemyModels = { modelFighterBody_.get(), modelFighterPHead_.get(), modelFighterL_arm_.get(),
			modelFighterR_arm_.get(),enemyWeaponModel_.get() };

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModels);

	std::vector<Model*> playerModels = { modelFighterBody_.get(), modelFighterPHead_.get(), modelFighterL_arm_.get(),
		modelFighterR_arm_.get() };
	
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);
	player_->SetEnemy(enemy_.get());

	enemy_->SetPlayer(player_.get());

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
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

	if (input_->PushKey(DIK_1))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(true);
	}

	if (input_->PushKey(DIK_2))
	{
		PostProcess::GetInstance()->SetIsBloomActive(true);
	}

	if (input_->PushKey(DIK_3))
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}

	if (input_->PushKey(DIK_4))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(false);
		PostProcess::GetInstance()->SetIsBloomActive(false);
		PostProcess::GetInstance()->SetIsVignetteActive(false);
	}

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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
			PlayerWinCount_ = 2;
		}
	}

	if (PlayerWinCount_ == 2)
	{
		sceneManager->ChangeScene(new GameEndScene);
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
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
			player_->SetHP(100);
			enemy_->SetHP(100);
			player_->SetTransform({ -7.0f,0.0f,0.0f });
			player_->SetRotation({ 0.0f,1.7f,0.0f });
			enemy_->SetTransform({ 7.0f,0.0f,0.0f });
			enemy_->SetRotation({ 0.0f,4.6f,0.0f });
			EnemyWinCount_ = 2;
		}
	}

	if (EnemyWinCount_ == 2)
	{
		sceneManager->ChangeScene(new GameEndScene);
	}

	ImGui::Begin("round");
	ImGui::Text("round %d", round_);
	ImGui::Text("PlayerWinCount %d", PlayerWinCount_);
	ImGui::Text("EnemyWinCount %d", EnemyWinCount_);
	ImGui::End();

	camera_.UpdateMatrix();

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
};

void GamePlayScene::Draw(SceneManager* sceneManager)
{

	/*Model::PreDraw();

	player_->Draw(camera_);

	Model::PostDraw();*/

	PostProcess::GetInstance()->PreDraw();

	DirectXCore::GetInstance()->ClearDepthBuffer();

	Model::PreDraw();

	player_->Draw(camera_);

	enemy_->Draw(camera_);

	skydome_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};