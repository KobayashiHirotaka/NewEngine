#include "Application/Game/Scenes/Manager/GameManager.h"

/**
 * @file main.cpp
 * @brief ゲームの実行を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	EngineCore* game = new GameManager();
	game->Run();
	delete game;
	return 0;
}