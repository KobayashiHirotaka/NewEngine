/**
 * @file SceneManager.cpp
 * @brief 各シーンの管理(更新、描画、シーン遷移など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "SceneManager.h"
#include "Engine/Utility/GlobalVariables.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	return instance_;
}

void SceneManager::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

SceneManager::SceneManager()
{
	currentScene_ = {};
	nextScene_ = {};
}

SceneManager::~SceneManager()
{
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

	currentScene_->Update();

	currentScene_->ImGui();
}

void SceneManager::Draw()
{
	currentScene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}