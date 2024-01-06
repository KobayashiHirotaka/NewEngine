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

	audio_ = Audio::GetInstance();
	audio_->Initialize();

	input_ = Input::GetInstance();
	input_->Initialize(win_);

	postProcess_ = PostProcess::GetInstance();
	postProcess_->Initialize();

	Model::StaticInitialize();

	ParticleModel::StaticInitialize();

	Sprite::StaticInitialize();

	GlobalVariables::GetInstance()->LoadFiles();

	Random::Initialize();

	//シーンの初期化
	currentScene_ = new GameStartScene();
	currentScene_->Initialize(this);
}

SceneManager::~SceneManager()
{
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
		//Windowのメッセージ処理
		if (win_->ProcessMessage()) 
		{
			break;
		}

		//ゲームの処理
		imguiManager_->BeginFlame();

		input_->Update();

		//GlobalVariables::GetInstance()->Update();

		currentScene_->Update(this);

		postProcess_->Update();

		imguiManager_->EndFlame();

		dxCore_->PreDraw();

		currentScene_->Draw(this);

		imguiManager_->Draw();

		dxCore_->PostDraw();
	}

	imguiManager_->ShutDown();

	//audio_->Finalize();

	win_->CloseGameWindow();
}