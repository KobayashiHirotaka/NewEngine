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
	//引数として渡された値を使わない場合、警告を防ぐために無視
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;

	//EngineCoreインスタンスを作成
	EngineCore* game = new GameManager();

	//ゲームを実行
	game->Run();

	//解放
	delete game;

	//終了
	return 0;
}