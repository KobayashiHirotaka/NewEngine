#include "SceneManager.h"
#include "Project/Game/Scenes/GameTitleScene.h"
#include "Project/Game/Scenes/GamePlayScene.h"
#include "Project/Game/Scenes/GameClearScene.h"
#include "Engine/Utility/GlobalVariables.h"

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
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
	currentScene_ = nullptr;
	currentScene_ = newScene;
	currentScene_->Initialize(this);
}