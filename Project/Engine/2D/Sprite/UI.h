
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
	//描画するか
	bool isDraw_ = true;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//位置
	Vector2 position_{};

	//回転
	float rotation_{};

	//サイズ
	Vector2 size_{};

	//Spriteのポインタ
	Sprite* sprite_ = nullptr;;
};
