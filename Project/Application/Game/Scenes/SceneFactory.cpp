/**
 * @file SceneFactory.cpp
 * @brief シーンの管理(作成)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "SceneFactory.h"

IScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	IScene* newScene = nullptr;

	if (sceneName == "GameTitleScene") 
	{
		newScene = new GameTitleScene();
	}
	else if (sceneName == "GamePlayScene")
	{
		newScene = new GamePlayScene();
	}
	else if (sceneName == "GameWinScene") 
	{
		newScene = new GameWinScene();
	}
	else if (sceneName == "GameLoseScene")
	{
		newScene = new GameLoseScene();
	}
	
	return newScene;
}