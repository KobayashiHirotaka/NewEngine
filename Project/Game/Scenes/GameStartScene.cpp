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

	skydomeModel_.reset(Model::CreateFromOBJ("resource/skydome", "skydome.obj"));

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

	skydome_->Update();

	if (input_->IsPushKey(DIK_SPACE))
	{
		sceneManager->ChangeScene(new GamePlayScene);
	}

	if (input_->IsPushKey(DIK_L))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(true);
		PostProcess::GetInstance()->SetIsBloomActive(true);
	}

	if (input_->IsPushKey(DIK_K))
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

	skydome_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};