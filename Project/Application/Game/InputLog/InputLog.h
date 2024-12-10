/**
 * @file InputLog.cpp
 * @brief 入力履歴表示、経過フレームの表示を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Input/Input.h"
#include <deque>
#include <algorithm>

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
	uint32_t stickTextureHandle_[9];

	//ボタン入力のSprite
	std::vector<std::unique_ptr<Sprite>> buttonSprites_;
	uint32_t buttonTextureHandle_[6];

	//数字のSprite
	std::vector<std::unique_ptr<Sprite>> numberTensSprites_;
	std::vector<std::unique_ptr<Sprite>> numberOnesSprites_;
	uint32_t digitTextureHandles_[10];

	//ボタン入力を保持
	std::deque<std::pair<int, int>> inputHistory_; 

	//経過フレーム数を保持
	std::deque<int> frameCounts_;

	//表示する履歴の最大数
	const int kMaxHistorySize_ = 8; 

	//縦の間隔
	const float kVerticalSpacing_ = 50.0f; 

	//操作表示のX座標
	const float kLeftPositionX_ = 70.0f;   

	//ボタン表示のX座標
	const float kRightPositionX_ = 110.0f;  

	//経過フレーム表示(10の位)のX座標
	const float kNumberTensPositionX_ = 10.0f;

	//経過フレーム表示(1の位)のX座標
	const float kNumberOnesPositionX_ = 30.0f;

	//初期のY座標
	const float kBasePositionY_ = 150.0f; 

	//stickのしきい値
	const float kValue_ = 0.7f;
};

