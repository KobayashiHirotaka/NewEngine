#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	modelManager_ = ModelManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//PostProcess::GetInstance()->SetIsPostProcessActive(true);
	//PostProcess::GetInstance()->SetIsBloomActive(true);
	//PostProcess::GetInstance()->SetIsVignetteActive(true);

	modelManager_->LoadModel("resource/hammer", "hammer.obj");
	modelManager_->LoadModel("resource/skydome", "skydome.obj");

	worldTransform_.Initialize();
	worldTransform_.translation = { 0.0f,-1.0f,0.0f };

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetModel(modelManager_->FindModel("hammer.obj"));

	camera_.UpdateMatrix();

	titleSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Title.wav");
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayWave(titleSoundHandle_, true, 1.0f);
};

void GameTitleScene::Update()
{
	player_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GamePlayScene");
		return;
	}

	if (input_->PushKey(DIK_RETURN))
	{
		player_->SetModel(modelManager_->FindModel("skydome.obj"));
	}

	worldTransform_.UpdateMatrix();

	camera_.UpdateMatrix();

	ImGui::Begin("TitleScene");
	ImGui::Text("Abutton or SpaceKey : PlayScene");
	ImGui::End();
};

void GameTitleScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	//PostProcess::GetInstance()->PreDraw();

	player_->Draw(camera_);

	Model::PreDraw();

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	//PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GameTitleScene::Finalize()
{

}