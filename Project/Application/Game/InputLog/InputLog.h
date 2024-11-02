#pragma once
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Input/Input.h"

class InputLog
{
public:
	void Initialize();

	void Update();

	void Draw();

private:
	Input* input_ = nullptr;

	//スティック入力のSprite
	std::unique_ptr<Sprite>stickSprite_[8];
	uint32_t stickTextureHandle_[8];

	//ボタン入力のSprite
	std::unique_ptr<Sprite>buttonSprite_[6];
	uint32_t buttonTextureHandle_[6];


};

