/**
 * @file main.cpp
 * @brief ゲームの実行を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Application/Game/Scenes/Manager/GameManager.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;

	EngineCore* game = new GameManager();
	game->Run();
	delete game;
	return 0;
}