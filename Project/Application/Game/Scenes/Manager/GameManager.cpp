#include "GameManager.h"

void GameManager::Initialize()
{
	EngineCore::Initialize();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("GamePlayScene");
}

//void GameManager::Finalize()
//{
//	EngineCore::Finalize();
//}

//void GameManager::Update()
//{
//	EngineCore::Update();
//}

void GameManager::Draw()
{
	EngineCore::Draw();
}

