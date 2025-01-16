/**
 * @file SceneFactory.h
 * @brief シーンの生成を行うファクトリークラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Framework/AbstractSceneFactory.h"
#include "Application/Game/Scenes/GameTitleScene/GameTitleScene.h"
#include "Application/Game/Scenes/GamePlayScene/GamePlayScene.h"
#include "Application/Game/Scenes/GameWinScene/GameWinScene.h"
#include "Application/Game/Scenes/GameLoseScene/GameLoseScene.h"

class SceneFactory : public AbstractSceneFactory 
{
public:
	/// <summary>シーンの生成</summary>
	IScene* CreateScene(const std::string& sceneName) override;
};

