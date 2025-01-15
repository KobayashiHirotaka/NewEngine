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

//スティック入力の方向
enum class StickDirection
{
	DownLeft,
	Down,
	DownRight,
	Left,
	Right,
	UpLeft,
	Up,
	UpRight,
	Neutral,
};

//ボタン入力
enum class Button
{
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	LB = 4
};


class InputLog
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw();

private:
	//Inputのポインタ
	Input* input_ = nullptr;

	//スティック入力
	static const int kMaxArrow_ = 9;
	std::vector<std::unique_ptr<Sprite>> stickSprites_;
	uint32_t stickTextureHandle_[kMaxArrow_];
	StickDirection stickDirection_;

	//ボタン入力
	std::vector<std::unique_ptr<Sprite>> buttonSprites_;
	uint32_t buttonTextureHandle_[6];
	Button button_;

	//数字のSprite
	static const int kMaxNum_ = 10;
	std::vector<std::unique_ptr<Sprite>> numberTensSprites_;
	std::vector<std::unique_ptr<Sprite>> numberOnesSprites_;
	uint32_t digitTextureHandle_[kMaxNum_];

	//ボタン入力を保持
	std::deque<std::pair<int, int>> inputHistory_; 

	//経過フレーム数を保持
	std::deque<int> frameCounts_;

	//画像サイズ
	Vector2 textureSize_ = { 40.0f,40.0f };

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

	//スティックの閾値
	const float kStickDeadZone_ = 0.7f;

	//入力がない
	const int kNoInput_ = -1;
};

