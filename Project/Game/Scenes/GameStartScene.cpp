#include "GameStartScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GamePlayScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameStartScene::GameStartScene() {};

GameStartScene::~GameStartScene() {};

void GameStartScene::Initialize(SceneManager* sceneManager)
{
	input_ = Input::GetInstance();

	camera_.Initialize();

	playerModel_.reset(Model::CreateFromOBJ("resource/player", "player.obj"));
	skydomeModel_.reset(Model::CreateFromOBJ("resource/skydome", "skydome.obj"));

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/float_Body", "float_Body.obj"));
	modelFighterHead_.reset(Model::CreateFromOBJ("resource/float_Head", "float_Head.obj"));
	modelFighterPHead_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("resource/float_L_arm", "float_L_arm.obj"));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("resource/float_R_arm", "float_R_arm.obj"));

	std::vector<Model*> playerModels = { modelFighterBody_.get(), modelFighterPHead_.get(), modelFighterL_arm_.get(),
			modelFighterR_arm_.get() };

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);

	std::vector<Model*> enemyModels = { modelFighterBody_.get(), modelFighterPHead_.get(), modelFighterL_arm_.get(),
			modelFighterR_arm_.get() };

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModels);

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	PostProcess::GetInstance()->SetIsPostProcessActive(false);
	PostProcess::GetInstance()->SetIsBloomActive(false);

	camera_.UpdateMatrix();
};

void GameStartScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Start");
	
	ImGui::End();

	player_->Update();

	enemy_->Update();

	skydome_->Update();

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager->ChangeScene(new GamePlayScene);
	}

	if (input_->PushKey(DIK_L))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(true);
		PostProcess::GetInstance()->SetIsBloomActive(true);
	}

	if (input_->PushKey(DIK_K))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(false);
		PostProcess::GetInstance()->SetIsBloomActive(false);
	}

	camera_.UpdateMatrix();
};

void GameStartScene::Draw(SceneManager* sceneManager)
{
	PostProcess::GetInstance()->PreDraw();

	DirectXCore::GetInstance()->ClearDepthBuffer();

	Model::PreDraw();

	player_->Draw(camera_);

	enemy_->Draw(camera_);

	skydome_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};