#include "InputLog.h"

void InputLog::Initialize()
{
	//Inputのinstance
	input_ = Input::GetInstance();

	//リソース
	//操作
	stickTextureHandle_[0] = TextureManager::LoadTexture("resource/images/arrow_1.png");
	stickTextureHandle_[1] = TextureManager::LoadTexture("resource/images/arrow_2.png");
	stickTextureHandle_[2] = TextureManager::LoadTexture("resource/images/arrow_3.png");
	stickTextureHandle_[3] = TextureManager::LoadTexture("resource/images/arrow_4.png");
	stickTextureHandle_[4] = TextureManager::LoadTexture("resource/images/arrow_6.png");
	stickTextureHandle_[5] = TextureManager::LoadTexture("resource/images/arrow_7.png");
	stickTextureHandle_[6] = TextureManager::LoadTexture("resource/images/arrow_8.png");
	stickTextureHandle_[7] = TextureManager::LoadTexture("resource/images/arrow_9.png");

	//ボタン
	buttonTextureHandle_[0] = TextureManager::LoadTexture("resource/images/A.png");
	buttonTextureHandle_[1] = TextureManager::LoadTexture("resource/images/B.png");
	buttonTextureHandle_[2] = TextureManager::LoadTexture("resource/images/X.png");
	buttonTextureHandle_[3] = TextureManager::LoadTexture("resource/images/Y.png");
	buttonTextureHandle_[4] = TextureManager::LoadTexture("resource/images/RB.png");
	buttonTextureHandle_[5] = TextureManager::LoadTexture("resource/images/LB.png");

	stickSprite_[0].reset(Sprite::Create(stickTextureHandle_[0], {40.0f,200.0f}));
	stickSprite_[0]->SetSize({ 55.0f, 55.0f });

	buttonSprite_[0].reset(Sprite::Create(buttonTextureHandle_[0], { 90.0f,200.0f }));
	buttonSprite_[0]->SetSize({ 55.0f, 55.0f });
}

void InputLog::Update()
{
	//操作表示(テスト)
	if (input_->PushKey(DIK_A))
	{
		stickSprite_[0]->SetTexture(stickTextureHandle_[3]);
	}

	if (input_->PushKey(DIK_D))
	{
		stickSprite_[0]->SetTexture(stickTextureHandle_[4]);
	}

	if (input_->PushKey(DIK_S))
	{
		stickSprite_[0]->SetTexture(stickTextureHandle_[1]);
	}

	//ボタン表示(テスト)
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		buttonSprite_[0]->SetTexture(buttonTextureHandle_[0]);
	}

	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		buttonSprite_[0]->SetTexture(buttonTextureHandle_[1]);
	}

	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		buttonSprite_[0]->SetTexture(buttonTextureHandle_[2]);
	}

	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
	{
		buttonSprite_[0]->SetTexture(buttonTextureHandle_[3]);
	}
}

void InputLog::Draw()
{
	stickSprite_[0]->Draw();

	buttonSprite_[0]->Draw();
}

