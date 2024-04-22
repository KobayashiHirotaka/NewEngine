#include "Application/Game/Scenes/Manager/SceneManager.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	SceneManager* sceneManager = new SceneManager();
	sceneManager->Run();
	delete sceneManager;
	return 0;
}