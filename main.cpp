#include "Project/Game/Scenes/Manager/SceneManager.h"

int IScene::sceneNum;

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SceneManager* sceneManager = new SceneManager();
	sceneManager->Run();

	return 0;
}