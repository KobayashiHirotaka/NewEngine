/**
 * @file SceneFactory.cpp
 * @brief シーンの生成を行うファクトリークラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "SceneFactory.h"

IScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//新しいシーン
	IScene* newScene = nullptr;

	//シーン名に応じて、対応するシーンを作成
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