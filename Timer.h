#pragma once
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include <memory>

class Timer
{
public:
	enum class DigitPlace
	{
		kOnes,
		kTens,
		kHundreds,
	};

public:
	Timer();
	~Timer();

	void Initialize(DigitPlace digit, uint32_t start, Vector3 translation);
	void Update();
	void Draw();
	void Set(uint32_t start);
	bool GetIsFin()const {return IsFin;};
private:
	bool IsFin = false;

	Sprite* sprite_;

	WorldTransform worldTransform_;

	uint32_t textureHandle_[10];
	uint32_t timer_ = 0;
	uint32_t second = 0;
	DigitPlace digitPlace_;
};