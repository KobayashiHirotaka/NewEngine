#include "SceneManager.h"
#include "Project/Game/Scenes/GameStartScene.h"
#include "Project/Game/Scenes/GamePlayScene.h"
#include "Project/Game/Scenes/GameEndScene.h"
#include "Engine/Utility/GlobalVariables.h"

SceneManager::SceneManager()
{
	win_ = WindowsApp::GetInstance();
	win_->CreateGameWindow(L"Engine", win_->kClientWidth, win_->kClientHeight);

	dxCore_ = DirectXCore::GetInstance();
	dxCore_->Initialize();

	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize();

	/*audio_ = Audio::GetInstance();
	audio_->Initialize();*/

	input_ = Input::GetInstance();
	input_->Initialize();

	postProcess_ = PostProcess::GetInstance();
	postProcess_->Initialize();

	Model::StaticInitialize();

	Sprite::StaticInitialize();

	GlobalVariables::GetInstance()->LoadFiles();

	//シーンの初期化
	currentScene_ = new GameStartScene();
	currentScene_->Initialize(this);
}

SceneManager::~SceneManager()
{
	//シーンの削除
	delete currentScene_;
	currentScene_ = nullptr;

	Model::Release();
	Sprite::Release();
}

void SceneManager::ChangeScene(IScene* newScene)
{
	currentScene_ = nullptr;
	currentScene_ = newScene;
	currentScene_->Initialize(this);
}

void SceneManager::Run() 
{

	while (true)
	{
		//メッセージ処理
		if (win_->ProcessMessage()) 
		{
			break;
		}

		//ImGui受付開始
		imguiManager_->BeginFlame();
		//Inputの更新
		input_->Update();
		//グローバル変数の更新
		//GlobalVariables::GetInstance()->Update();
		//ゲームシーンの更新
		currentScene_->Update(this);
		//ポストプロセスの更新
		postProcess_->Update();
		//ImGui受付終了
		imguiManager_->EndFlame();

		//描画開始
		dxCore_->PreDraw();
		//ゲームシーンの描画
		currentScene_->Draw(this);
		//ImGuiの描画
		imguiManager_->Draw();
		//描画終了
		dxCore_->PostDraw();
	}
	//ImGuiの解放処理
	imguiManager_->ShutDown();
	////Audioの解放処理
	//audio_->Finalize();
	//ゲームウィンドウ削除
	win_->CloseGameWindow();
}