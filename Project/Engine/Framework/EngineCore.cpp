/**
 * @file EngineCore.cpp
 * @brief エンジン機能の管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EngineCore.h"

D3DResourceLeakChecker EngineCore::sLeakCheck_;

void EngineCore::Initialize()
{
	//ウィンドウの生成と初期化
	win_ = WindowsApp::GetInstance();
	win_->CreateGameWindow(L"モノクロファイター", win_->kClientWidth, win_->kClientHeight);

	//DirectXCoreの初期化
	dxCore_ = DirectXCore::GetInstance();
	dxCore_->Initialize();

	//TextureManagerの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	//ImGuiManagerの初期化
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize();

	//DirectXCoreの初期化
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	//Inputの初期化
	input_ = Input::GetInstance();
	input_->Initialize(win_);

	//PostProcessの初期化
	postProcess_ = PostProcess::GetInstance();
	postProcess_->Initialize();

	//SceneManagerのインスタンス
	sceneManager_ = SceneManager::GetInstance();

	//静的リソースの初期化
	Model::StaticInitialize();
	ParticleModel::StaticInitialize();
	Skybox::StaticInitialize();
	Line::StaticInitialize();
	Sprite::StaticInitialize();

	//GlobalVariablesのロード
	GlobalVariables::GetInstance()->LoadFiles();

	//AttackEditorのロード
	AttackEditor::GetInstance()->Initialize();

	//ゲームオブジェクト管理の準備
	game3dObjectFactory_ = std::make_unique<Game3dObjectFactory>();
	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->SetGameObjectFactory(game3dObjectFactory_.get());

	// レベルローダーのインスタンス
	levelLoader_ = LevelLoader::GetInstance();

	//ランダムシードの初期化
	Random::Initialize();
}

void EngineCore::Finalize()
{
	//静的リソースの解放
	Model::Release();
	Sprite::Release();
	ParticleModel::Release();
	Skybox::Release();
	Line::Release();

	//ModelManagerのインスタンスを削除
	ModelManager::DeleteInstance();

	//PostProcessのインスタンスを削除
	PostProcess::DeleteInstance();

	//Game3dObjectManagerのインスタンスを削除
	Game3dObjectManager::DeleteInstance();

	//LevelLoaderのインスタンスを削除
	LevelLoader::DeleteInstance();

	//Inputのインスタンスを削除
	Input::DeleteInstance();

	//Audioの解放
	audio_->Release();

	//SceneManagerのインスタンスを削除
	SceneManager::DeleteInstance();

	//ImGuiシステムを終了
	imguiManager_->ShutDown();

	//TextureManagerのインスタンスを削除
	TextureManager::DeleteInstance();

	//DirectXCoreのインスタンスを削除
	DirectXCore::DeleteInstance();

	//ウィンドウを閉じる
	win_->CloseGameWindow();
}


void EngineCore::Update()
{
	//ゲームの処理(更新処理)
	imguiManager_->BeginFlame();

	sceneManager_->Update();

	input_->Update();

	//GlobalVariables::GetInstance()->Update();

	AttackEditor::GetInstance()->Update();

	postProcess_->Update();

	GameTimer::Update();

	imguiManager_->EndFlame();
}

void EngineCore::Draw()
{
	//描画処理
	dxCore_->PreDraw();

	sceneManager_->Draw();

	imguiManager_->Draw();

	dxCore_->PostDraw();
}

bool EngineCore::IsEndRequst()
{
	//ウィンドウのメッセージ処理を行い、終了リクエストをチェック
	endRequst_ = win_->ProcessMessage();
	return endRequst_;
}

void EngineCore::Run()
{
	//初期化
	Initialize();

	//ゲームループ
	while (true)
	{
		//Windowのメッセージ処理
		if (IsEndRequst())
		{
			break;
		}

		//更新
		Update();

		//描画
		Draw();
	}

	//終了
	Finalize();
}

