/**
 * @file BaseCharacterUI.h
 * @brief キャラクターのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/2D/Sprite/UI.h"

class BaseCharacterUI
{	
public:
	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw() = 0;

	//UIの更新
	/// <summary>HPのUIの更新</summary>
	virtual void UpdateHPBar() = 0;

protected:
	//コンボ数を描画するため
	const int kComboCount_ = 2;
};

