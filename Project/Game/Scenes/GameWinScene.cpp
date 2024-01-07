#include "GameWinScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameStartScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameWinScene::GameWinScene() {};

GameWinScene::~GameWinScene() {};

void GameWinScene::Initialize(SceneManager* sceneManager)
{
	input_ = Input::GetInstance();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	camera_.UpdateMatrix();
};

void GameWinScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Win");

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

void GameWinScene::Draw(SceneManager* sceneManager)
{
	PostProcess::GetInstance()->PreDraw();

	DirectXCore::GetInstance()->ClearDepthBuffer();

	Model::PreDraw();

	skydome_->Draw(camera_);

	Model::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};