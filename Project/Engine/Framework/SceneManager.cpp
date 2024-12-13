/**
 * @file SceneManager.cpp
 * @brief 各シーンの管理(更新、描画、シーン遷移など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "SceneManager.h"
#include "Engine/Utility/GlobalVariables.h"

SceneManager* SceneManager::sInstance_ = nullptr;

SceneManager* SceneManager::GetInstance()
{
	//インスタンスを生成
	if (sInstance_ == nullptr)
	{
		sInstance_ = new SceneManager();
	}
	return sInstance_;
}

void SceneManager::DeleteInstance()
{
	//インスタンスを削除
	if (sInstance_ != nullptr)
	{
		delete sInstance_;
		sInstance_ = nullptr;
	}
}

SceneManager::SceneManager()
{
	//現在のシーンと次のシーンを初期化
	currentScene_ = {};
	nextScene_ = {};
}

SceneManager::~SceneManager()
{
	//現在のシーンを削除
	delete currentScene_;
	currentScene_ = nullptr;
}

void SceneManager::Update()
{
	if (nextScene_)
	{
		//旧シーンの終了
		if (currentScene_)
		{
			currentScene_->Finalize();
			delete currentScene_;
		}

		//シーン切り替え
		currentScene_ = nextScene_;
		nextScene_ = nullptr;

		//シーンマネージャーをセット
		currentScene_->SetSceneManager(this);

		//シーンの初期化
		currentScene_->Initialize();
	}

	//現在のシーンの更新
	currentScene_->Update();

	//現在のシーンのImGui
	currentScene_->ImGui();
}

void SceneManager::Draw()
{
	//現在のシーンの描画
	currentScene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//シーンの生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}