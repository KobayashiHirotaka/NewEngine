/**
 * @file IGameSceneUI.h
 * @brief 各シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/12/31
 */

#pragma once
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"

//操作説明の種類
enum class CommandSpriteType
{
	//基本操作
	GeneralCommandSprite,

	//コンボ攻撃
	ComboAttackCommandSprite,

	//必殺技攻撃
	FinisherAttackCommandSprite,
};

class IGameSceneUI
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw() = 0;

protected:
	//Input
	Input* input_ = nullptr;

	//Audio
	Audio* audio_ = nullptr;
};

