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
			currentScene_->Finalize(this);
			delete currentScene_;
		}

		//シーン切り替え
		currentScene_ = nextScene_;
		nextScene_ = nullptr;

		//シーンマネージャーをセット
		currentScene_->SetSceneManager(this);

		//シーンの初期化
		currentScene_->Initialize(this);
	}

	currentScene_->Update(this);
}

void SceneManager::Draw()
{
	currentScene_->Draw(this);
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}