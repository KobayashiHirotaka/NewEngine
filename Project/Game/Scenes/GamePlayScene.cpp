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

	weaponModel_.reset(Model::CreateFromOBJ("resource/hammer", "hammer.obj"));

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/float_Body", "float_Body.obj"));
	modelFighterHead_.reset(Model::CreateFromOBJ("resource/float_Head", "float_Head.obj"));
	modelFighterPHead_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("resource/float_L_arm", "float_L_arm.obj"));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("resource/float_R_arm", "float_R_arm.obj"));

	std::vector<Model*> playerModels = { modelFighterBody_.get(), modelFighterPHead_.get(), modelFighterL_arm_.get(),
			modelFighterR_arm_.get(),weaponModel_.get() };

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);

	std::vector<Model*> enemyModels = { modelFighterBody_.get(), modelFighterPHead_.get(), modelFighterL_arm_.get(),
			modelFighterR_arm_.get() };

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModels);

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

	camera_.UpdateMatrix();

	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_.get());

	if (player_->GetWeapon()->GetIsAttack())
	{
		collisionManager_->AddCollider(player_->GetWeapon());
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