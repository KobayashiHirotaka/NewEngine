#include "SceneManager.h"
#include "Application/Game/Scenes/GameTitleScene.h"
#include "Application/Game/Scenes/GamePlayScene.h"
#include "Application/Game/Scenes/GameClearScene.h"
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
	//シーンの初期化
	currentScene_ = new GameTitleScene();
	currentScene_->Initialize(this);
}

SceneManager::~SceneManager()
{
	delete currentScene_;
	currentScene_ = nullptr;
}

void SceneManager::Update()
{
	currentScene_->Update(this);
}

void SceneManager::Draw()
{
	currentScene_->Draw(this);
}

void SceneManager::ChangeScene(IScene* newScene)
{
	delete currentScene_;
	currentScene_ = nullptr;
	currentScene_ = newScene;
	currentScene_->Initialize(this);
}