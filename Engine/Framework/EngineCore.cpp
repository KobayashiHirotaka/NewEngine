#include "EngineCore.h"

void EngineCore::Initialize()
{
	win_ = WindowsApp::GetInstance();
	win_->CreateGameWindow(L"LE2A_10_コバヤシ_ヒロタカ_モノクロファイター", win_->kClientWidth, win_->kClientHeight);

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

	sceneManager_ = SceneManager::GetInstance();

	Model::StaticInitialize();

	ParticleModel::StaticInitialize();

	Sprite::StaticInitialize();

	GlobalVariables::GetInstance()->LoadFiles();

	Random::Initialize();
}

void EngineCore::Finalize()
{
	Model::Release();

	Sprite::Release();

	ParticleModel::Release();

	imguiManager_->ShutDown();

	audio_->Release();

	win_->CloseGameWindow();
}


void EngineCore::Update()
{
	//ゲームの処理
	imguiManager_->BeginFlame();

	sceneManager_->Update();

	input_->Update();

	//GlobalVariables::GetInstance()->Update();

	postProcess_->Update();

	imguiManager_->EndFlame();
}

void EngineCore::Draw()
{
	dxCore_->PreDraw();

	imguiManager_->Draw();

	sceneManager_->Draw();

	dxCore_->PostDraw();
}

bool EngineCore::IsEndRequst()
{
	endRequst_ = win_->ProcessMessage();
	return endRequst_;
}

void EngineCore::Run()
{
	Initialize();

	while (true)
	{
		//Windowのメッセージ処理
		if (IsEndRequst())
		{
			break;
		}

		Update();

		Draw();
	}

	Finalize();
}
