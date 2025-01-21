/**
 * @file GameWinSceneUI.h
 * @brief 勝利シーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#pragma once
#include "Application/Game/Scenes/IGameSceneUI.h"

class GameWinSceneUI : public IGameSceneUI
{
public:
	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

private:
	//Win表示のSprite
	std::unique_ptr<Sprite> winSceneSprite_ = nullptr;
	uint32_t winSceneTextureHandle_ = 0;
};

