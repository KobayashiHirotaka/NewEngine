/**
 * @file IGameSceneUI.h
 * @brief 各シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2025/01/15
 */

#pragma once
#include "Engine/2D/Sprite/Sprite.h"

class IGameSceneUI
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~IGameSceneUI() = default;

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw() = 0;
};

