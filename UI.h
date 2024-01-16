#pragma once
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/2D/Sprite/Sprite.h"

struct UI
{
	bool isDraw_ = true;
	uint32_t textureHandle_ = 0;
	Vector2 position_{};
	float rotation_{};
	Vector2 size_{};
	Sprite* sprite_ = nullptr;;
};