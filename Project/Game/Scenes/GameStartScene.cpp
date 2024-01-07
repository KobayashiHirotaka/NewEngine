#include "GameStartScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GamePlayScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameStartScene::GameStartScene() {};

GameStartScene::~GameStartScene() {};

void GameStartScene::Initialize(SceneManager* sceneManager)
{
	textureManager_ = TextureManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	camera_.UpdateMatrix();

	titleTextureHandle_ = TextureManager::Load("resource/title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { 0.0f,0.0f }));

	titleSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Title.wav");
	audio_->SoundPlayWave(titleSoundHandle_, true, 1.0f);

	currentSeconds_ = 99;
	UpdateNumberSprite();
};

void GameStartScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Start");
	ImGui::Text("%d", currentSeconds_);
	ImGui::End();

	skydome_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
		/*	audio_->StopAudio(titleSoundHandle_);*/
			sceneManager->ChangeScene(new GamePlayScene);
		}
	}

	// 時間経過を加算
	elapsedTime += frameTime;

	// タイムカウントを更新
	if (currentSeconds_ > 0 && elapsedTime >= 1.0f) {
		currentSeconds_--;
		UpdateNumberSprite();

		// elapsedTimeをリセット
		elapsedTime = 0.0f;
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

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	/*titleSprite_->Draw();*/

	numberOnesSprite_->Draw();
	numberTensSprite_->Draw();

	Sprite::PostDraw();
};

void GameStartScene::UpdateNumberSprite()
{
	int tensDigit = currentSeconds_ / 10;
	int onesDigit = currentSeconds_ % 10;

	tensTextureHandle_ = TextureManager::Load("resource/number/" + std::to_string(tensDigit) + ".png");
	onesTextureHandle_ = TextureManager::Load("resource/number/" + std::to_string(onesDigit) + ".png");

	// 10の位の数字スプライトの位置は左に少しずらす例
	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, { 0.0f, 0.0f }));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, { 40.0f, 0.0f }));
}