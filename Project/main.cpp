/**
 * @file main.cpp
 * @brief ゲームの実行を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Application/Game/Scenes/Manager/GameManager.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR, _In_ int)
{
	//EngineCoreインスタンスを作成
	EngineCore* game = new GameManager();

	//ゲームを実行
	game->Run();

	//解放
	delete game;

	//終了
	return 0;
}