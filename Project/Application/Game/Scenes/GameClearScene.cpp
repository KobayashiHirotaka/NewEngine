#include "GameClearScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameClearScene::GameClearScene() {};

GameClearScene::~GameClearScene() {};

void GameClearScene::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	modelManager_ = ModelManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	modelManager_->LoadModel("resource/hammer", "hammer.obj");
	modelManager_->LoadModel("resource/skydome", "skydome.obj");

	player_ = std::make_unique<Player>();
	player_->Initialize();

	debugCamera_.Initialize();

	camera_.UpdateMatrix();
};

void GameClearScene::Update()
{
	player_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameTitleScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
		return;
	}

	//ポストプロセス
	if (input_->PressKey(DIK_1))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(true);
	}

	//Bloom
	if (input_->PressKey(DIK_2))
	{
		PostProcess::GetInstance()->SetIsBloomActive(true);
	}

	//Vignette
	if (input_->PressKey(DIK_3))
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}

	if (input_->PressKey(DIK_4))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(false);
		PostProcess::GetInstance()->SetIsBloomActive(false);
		PostProcess::GetInstance()->SetIsVignetteActive(false);
	}

	debugCamera_.Update();

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

	if (isDebugCamera_)
	{
		camera_.matView_ = debugCamera_.GetCamera().matView_;
		camera_.matProjection_ = debugCamera_.GetCamera().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		camera_.UpdateMatrix();
	}

	ImGui::Begin("ClearScene");
	ImGui::Text("Abutton or SpaceKey : TitleScene");
	ImGui::End();
};

void GameClearScene::Draw()
{
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	player_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	player_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GameClearScene::Finalize()
{

}