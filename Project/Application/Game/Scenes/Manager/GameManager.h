#pragma once
#include "Engine/Framework/EngineCore.h"
#include "Application/Game/Scenes/SceneFactory.h"

/**
 * @file GameManager.h
 * @brief ゲーム全体の管理（初期化、描画など）を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

class GameManager : public EngineCore
{
public:
	void Initialize() override;

	//void Update()override;

	void Draw()override;

	//void Finalize()override;
};

