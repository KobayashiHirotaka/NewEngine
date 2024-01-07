#include "GameEndScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameStartScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameEndScene::GameEndScene() {};

GameEndScene::~GameEndScene() {};

void GameEndScene::Initialize(SceneManager* sceneManager)
{
	input_ = Input::GetInstance();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	camera_.UpdateMatrix();
};

void GameEndScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("End");

	ImGui::End();

	skydome_->Update();

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager->ChangeScene(new GameStartScene);
	}

	camera_.UpdateMatrix();
};

void GameEndScene::Draw(SceneManager* sceneManager)
{
	PostProcess::GetInstance()->PreDraw();

	DirectXCore::GetInstance()->ClearDepthBuffer();

	Model::PreDraw();

	skydome_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};