/**
 * @file AbstractSceneFactory.h
 * @brief シーン生成のためのファクトリークラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/Game/Scenes/IScene.h"
#include <string>

class AbstractSceneFactory
{
public:
	virtual ~AbstractSceneFactory() = default;

	virtual IScene* CreateScene(const std::string& sceneName) = 0;
};
