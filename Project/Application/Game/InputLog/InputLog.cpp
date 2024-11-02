#include "InputLog.h"

void InputLog::Initialize()
{
	//Inputのinstance
	input_ = Input::GetInstance();

	//リソース
	stickTextureHandle_[0] = TextureManager::LoadTexture("resource/images/arrow_1.png");
	stickTextureHandle_[1] = TextureManager::LoadTexture("resource/images/arrow_2.png");
	stickTextureHandle_[2] = TextureManager::LoadTexture("resource/images/arrow_3.png");
	stickTextureHandle_[3] = TextureManager::LoadTexture("resource/images/arrow_4.png");
	stickTextureHandle_[4] = TextureManager::LoadTexture("resource/images/arrow_6.png");
	stickTextureHandle_[5] = TextureManager::LoadTexture("resource/images/arrow_7.png");
	stickTextureHandle_[6] = TextureManager::LoadTexture("resource/images/arrow_8.png");
	stickTextureHandle_[7] = TextureManager::LoadTexture("resource/images/arrow_9.png");

	stickSprite_[0].reset(Sprite::Create(stickTextureHandle_[0], {40.0f,200.0f}));
	stickSprite_[0]->SetSize({ 55.0f, 55.0f });
}

void InputLog::Update()
{
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
}

void InputLog::Draw()
{
	stickSprite_[0]->Draw();
}

