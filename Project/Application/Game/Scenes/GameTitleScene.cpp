#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "GamePlayScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize(SceneManager* sceneManager)
{
	textureManager_ = TextureManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	light_ = std::make_unique<Light>();
	light_->Initialize();

	//PostProcess::GetInstance()->SetIsPostProcessActive(true);
	//PostProcess::GetInstance()->SetIsBloomActive(true);
	//PostProcess::GetInstance()->SetIsVignetteActive(true);

	model_.reset(Model::CreateFromOBJ("resource/models", "monsterBall.obj"));

	worldTransform_.Initialize();

	camera_.UpdateMatrix();

	titleSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Title.wav");
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayWave(titleSoundHandle_, true, 1.0f);
};

void GameTitleScene::Update(SceneManager* sceneManager)
{
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager->ChangeScene(new GamePlayScene);
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager->ChangeScene(new GamePlayScene);
		return;
	}

	model_->GetLight()->ImGui("Title");

	worldTransform_.UpdateMatrix();

	camera_.UpdateMatrix();

	ImGui::Begin("TitleScene");
	ImGui::Text("Abutton or SpaceKey : PlayScene");
	ImGui::End();
};

void GameTitleScene::Draw(SceneManager* sceneManager)
{
	Model::PreDraw();

	Model::PostDraw();

	//PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	model_->Draw(worldTransform_, camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	//PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};