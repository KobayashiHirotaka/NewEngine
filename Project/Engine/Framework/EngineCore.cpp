#include "EngineCore.h"

D3DResourceLeakChecker EngineCore::leakCheck;

void EngineCore::Initialize()
{
	win_ = WindowsApp::GetInstance();
	win_->CreateGameWindow(L"モノクロファイター", win_->kClientWidth, win_->kClientHeight);

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

	Skybox::StaticInitialize();

	Line::StaticInitialize();

	Sprite::StaticInitialize();

	GlobalVariables::GetInstance()->LoadFiles();

	AttackEditor::GetInstance()->Initialize();

	game3dObjectFactory_ = std::make_unique<Game3dObjectFactory>();
	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->SetGameObjectFactory(game3dObjectFactory_.get());

	levelLoader_ = LevelLoader::GetInstance();

	Random::Initialize();
}

void EngineCore::Finalize()
{
	Model::Release();

	Sprite::Release();

	ParticleModel::Release();

	ModelManager::DeleteInstance();

	Skybox::Release();

	Line::Release();

	PostProcess::DeleteInstance();

	Game3dObjectManager::DeleteInstance();

	LevelLoader::DeleteInstance();

	Input::DeleteInstance();

	audio_->Release();

	SceneManager::DeleteInstance();

	imguiManager_->ShutDown();

	TextureManager::DeleteInstance();

	DirectXCore::DeleteInstance();

	win_->CloseGameWindow();
}


void EngineCore::Update()
{
	//ゲームの処理
	imguiManager_->BeginFlame();

	sceneManager_->Update();

	input_->Update();

	//GlobalVariables::GetInstance()->Update();

	AttackEditor::GetInstance()->Update();

	postProcess_->Update();

	imguiManager_->EndFlame();
}

void EngineCore::Draw()
{
	dxCore_->PreDraw();

	sceneManager_->Draw();

	imguiManager_->Draw();

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
