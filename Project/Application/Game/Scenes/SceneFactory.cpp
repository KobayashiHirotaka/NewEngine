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
	else if (sceneName == "GameClearScene") {
		newScene = new GameClearScene();
	}
	
	return newScene;
}