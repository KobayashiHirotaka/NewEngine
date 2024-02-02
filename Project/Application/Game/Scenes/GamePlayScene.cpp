#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//PostProcess::GetInstance()->SetIsPostProcessActive(true);
	//PostProcess::GetInstance()->SetIsBloomActive(true);
	//PostProcess::GetInstance()->SetIsVignetteActive(true);

	camera_.UpdateMatrix();
};

void GamePlayScene::Update()
{
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameClearScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameClearScene");
		return;
	}

	camera_.UpdateMatrix();

	ImGui::Begin("PlayScene");
	ImGui::Text("Abutton or SpaceKey : ClearScene");
	ImGui::End();
};

void GamePlayScene::Draw()
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

void GamePlayScene::Finalize()
{

}