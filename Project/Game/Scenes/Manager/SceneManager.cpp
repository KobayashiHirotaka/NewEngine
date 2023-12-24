#include "SceneManager.h"
#include "Engine/Utility/GlobalVariables.h"

void SceneManager::Run()
{
	Initialize();
	Update();
	Release();
}

void SceneManager::Initialize()
{
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	win_ = WindowsApp::GetInstance();

	dxCore_ = DirectXCore::GetInstance();

	engine_ = MyEngine::GetInstance();
	engine_->Initialize();

	textureManager_ = TextureManager::GetInstance();

	imGuiManager_ = ImGuiManager::GetInstance();
	imGuiManager_->Initialize(win_, dxCore_);

	input_ = Input::GetInstance();
	input_->Initialize(win_);

	audio_ = Audio::GetInstance();
	audio_->Initialize();

	light_ = Light::GetInstance();
	light_->Initialize();

	GlobalVariables::GetInstance()->LoadFiles();

	state[START] = std::make_unique<GameStartScene>();
	state[PLAY] = std::make_unique<GamePlayScene>();
	state[PLAY]->Initialize();

	IScene::sceneNum = START;
}

void SceneManager::Update()
{
	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		//windowのメッセージを最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}else {
			//ゲームの処理
			imGuiManager_->BeginFlame();
			dxCore_->PreDraw();
			input_->Update();
			//GlobalVariables::GetInstance()->Update();

			state[GameStartScene::sceneNum]->Update();
			state[GameStartScene::sceneNum]->Draw();

			imGuiManager_->EndFlame();
			dxCore_->PostDraw();
		}
	}
}

void SceneManager::Release()
{
	dxCore_->Release();
}