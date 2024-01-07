#include "Timer.h"
#include "Engine/Base/TextureManager/TextureManager.h"

Timer::Timer() 
{
	sprite_ = new Sprite();

	for (uint32_t i = 0; i < 10; i++)
	{
		textureHandle_[i] = 0;
	}

	digitPlace_ = DigitPlace::kOnes;
}

Timer::~Timer() {


}

void Timer::Initialize(DigitPlace digit, uint32_t start, Vector3 translation)
{
	for (int i = 0; i < 10; i++)
	{
		sprite_->Create(textureHandle_[0],{0.0f,0.0f});
	}

	worldTransform_.Initialize();
	worldTransform_.translation = translation;
	worldTransform_.UpdateMatrix();


	digitPlace_ = digit;

	timer_ = start;
	second = 0;

	textureHandle_[0] = TextureManager::Load("resource/number/0.png");
	textureHandle_[1] = TextureManager::Load("resource/number/1.png");
	textureHandle_[2] = TextureManager::Load("resource/number/2.png");
	textureHandle_[3] = TextureManager::Load("resource/number/3.png");
	textureHandle_[4] = TextureManager::Load("resource/number/4.png");
	textureHandle_[5] = TextureManager::Load("resource/number/5.png");
	textureHandle_[6] = TextureManager::Load("resource/number/6.png");
	textureHandle_[7] = TextureManager::Load("resource/number/7.png");
	textureHandle_[8] = TextureManager::Load("resource/number/8.png");
	textureHandle_[9] = TextureManager::Load("resource/number/9.png");

}

void Timer::Update() {

	if (timer_ == 0) {
		second = 0;
		IsFin = true;
	}
	else {
		if (++second % 60 == 0) {
			timer_--;
		}

		if (timer_ < 15) {
			sprite_->SetColor(Vector4{ 1.0f, 0.0f, 0.0f, 1.0f });
		}
	}
}

void Timer::Draw() {
	switch (digitPlace_)
	{
	case Timer::DigitPlace::kOnes:
		sprite_->Draw();
		break;
	case Timer::DigitPlace::kTens:
		sprite_->Draw();
		break;
	case Timer::DigitPlace::kHundreds:
		//sprite_->Draw(worldTransform_, textureHandle_[timer_ / 100]);
		break;
	}
}

void Timer::Set(uint32_t start) {
	start;
}