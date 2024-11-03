#pragma once
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Input/Input.h"
#include <deque>

class InputLog
{
public:
	void Initialize();

	void Update();

	void Draw();

private:
	Input* input_ = nullptr;

	//スティック入力のSprite
	std::vector<std::unique_ptr<Sprite>> stickSprites_;
	uint32_t stickTextureHandle_[8];

	//ボタン入力のSprite
	std::vector<std::unique_ptr<Sprite>> buttonSprites_;
	uint32_t buttonTextureHandle_[6];

	//ボタン入力を保持
	std::deque<std::pair<int, int>> inputHistory_; 

	//表示する履歴の最大数
	const int maxHistorySize_ = 8; 

	//縦の間隔
	const float verticalSpacing_ = 60.0f; 

	//操作表示のX座標
	const float leftPositionX_ = 40.0f;   

	//ボタン表示のX座標
	const float rightPositionX_ = 100.0f;  

	//初期のY座標
	const float basePositionY_ = 130.0f; 

	//stickのしきい値
	const float value_ = 0.7f;
};

