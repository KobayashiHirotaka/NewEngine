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

	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);
	//PostProcess::GetInstance()->SetIsVignetteActive(true);

	camera_.UpdateMatrix();

	titleTextureHandle_ = TextureManager::LoadTexture("resource/title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { titleSpritePosition_.x, titleSpritePosition_.y}));

	titleUITextureHandle_ = TextureManager::LoadTexture("resource/titleUI.png");
	titleUISprite_.reset(Sprite::Create(titleUITextureHandle_, { 0.0f,0.0f }));

	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/GeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	attackCommandListTextureHandle_ = TextureManager::LoadTexture("resource/AttackCommandList.png");
	attackCommandListSprite_.reset(Sprite::Create(attackCommandListTextureHandle_, { 0.0f,0.0f }));

	titleSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Title.wav");
	selectSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Select.wav");
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayWave(titleSoundHandle_, true, 1.0f);
};

void GameStartScene::Update(SceneManager* sceneManager)
{
	titleSpriteMoveTimer_ --;

	float startY = titleSpritePosition_.y;
	float endY = startY + titleSpriteMoveSpeed_;

	float interpolatedY = Lerp(startY, endY, 0.4f);

	titleSpritePosition_.y = interpolatedY;

	if (titleSpriteMoveTimer_ < 0)
	{
		titleSpriteMoveSpeed_ *= -1.0f;
		titleSpriteMoveTimer_ = 30;
	}

	titleSprite_->SetPosition(titleSpritePosition_);

	skydome_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !isOpen_)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			sceneManager->ChangeScene(new GamePlayScene);
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
		sceneManager->ChangeScene(new GamePlayScene);
	}

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !isOpen_)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			isOpen_ = true;
			spriteCount_ = 1;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isOpen_)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			isOpen_ = false;
			spriteCount_ = 0;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) && isOpen_ && spriteCount_ == 1)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			spriteCount_ = 2;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) && isOpen_ && spriteCount_ == 2)
		{
			audio_->SoundPlayWave(selectSoundHandle_, false, 1.0f);
			spriteCount_ = 1;
		}
	}

	camera_.UpdateMatrix();

	ImGui::Begin("Text");
	ImGui::End();

	titleSprite_->ImGui("Sprite");
};

void GameStartScene::Draw(SceneManager* sceneManager)
{
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	skydome_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	if (!isOpen_)
	{
		titleSprite_->Draw();
	}

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	if (!isOpen_)
	{
		titleUISprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 1)
	{
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 2)
	{
		attackCommandListSprite_->Draw();
	}


	Sprite::PostDraw();
};