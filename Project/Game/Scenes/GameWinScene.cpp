#include "GameWinScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameStartScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameWinScene::GameWinScene() {};

GameWinScene::~GameWinScene() {};

void GameWinScene::Initialize(SceneManager* sceneManager)
{
	textureManager_ = TextureManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);
	//PostProcess::GetInstance()->SetIsVignetteActive(true);

	camera_.UpdateMatrix();

	winSceneTextureHandle_ = TextureManager::LoadTexture("resource/WinScene.png");
	winSceneSprite_.reset(Sprite::Create(winSceneTextureHandle_, { 0.0f,0.0f }));

	selectSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Select.wav");
};

void GameWinScene::Update(SceneManager* sceneManager)
{
	skydome_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			sceneManager->ChangeScene(new GameStartScene);
		}
	}

	camera_.UpdateMatrix();
};

void GameWinScene::Draw(SceneManager* sceneManager)
{
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	skydome_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	winSceneSprite_->Draw();

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();
};