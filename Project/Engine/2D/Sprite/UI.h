
/**
 * @file UI.h
 * @brief UIの構造体
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Sprite.h"
#include "Engine/Utility/Math/MyMath.h"

struct UI
{
	bool isDraw_ = true;
	uint32_t textureHandle_ = 0;
	Vector2 position_{};
	float rotation_{};
	Vector2 size_{};
	Sprite* sprite_ = nullptr;;
};
