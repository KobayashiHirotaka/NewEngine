#include "GameLoseScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameStartScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameLoseScene::GameLoseScene() {};

GameLoseScene::~GameLoseScene() {};

void GameLoseScene::Initialize(SceneManager* sceneManager)
{
	input_ = Input::GetInstance();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	camera_.UpdateMatrix();
};

void GameLoseScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Lose");

	ImGui::End();

	skydome_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager->ChangeScene(new GameStartScene);
		}
	}

	camera_.UpdateMatrix();
};

void GameLoseScene::Draw(SceneManager* sceneManager)
{
	PostProcess::GetInstance()->PreDraw();

	DirectXCore::GetInstance()->ClearDepthBuffer();

	Model::PreDraw();

	skydome_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};