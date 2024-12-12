/**
 * @file GameManager.h
 * @brief ゲーム全体の管理（初期化、描画など）を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Framework/EngineCore.h"
#include "Application/Game/Scenes/SceneFactory.h"

class GameManager : public EngineCore
{
public:
	/// <summary>初期化</summary>
	void Initialize() override;

	/// <summary>描画</summary>
	void Draw()override;
};

