#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "GameClearScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize(SceneManager* sceneManager)
{
	textureManager_ = TextureManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//PostProcess::GetInstance()->SetIsPostProcessActive(true);
	//PostProcess::GetInstance()->SetIsBloomActive(true);
	//PostProcess::GetInstance()->SetIsVignetteActive(true);

	camera_.UpdateMatrix();
};

void GamePlayScene::Update(SceneManager* sceneManager)
{
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager->ChangeScene(new GameClearScene);
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager->ChangeScene(new GameClearScene);
		return;
	}

	camera_.UpdateMatrix();

	ImGui::Begin("PlayScene");
	ImGui::Text("Abutton or SpaceKey : ClearScene");
	ImGui::End();
};

void GamePlayScene::Draw(SceneManager* sceneManager)
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	Model::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};