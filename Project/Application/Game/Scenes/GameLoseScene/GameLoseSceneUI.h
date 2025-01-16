/**
 * @file GameLoseSceneUI.h
 * @brief 敗北シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#pragma once
#include "Application/Game/Scenes/IGameSceneUI.h"

class GameLoseSceneUI : public IGameSceneUI
{
public:
	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

private:
	//Lose表示のSprite
	std::unique_ptr<Sprite> loseSceneSprite_ = nullptr;
	uint32_t loseSceneTextureHandle_ = 0;
};
