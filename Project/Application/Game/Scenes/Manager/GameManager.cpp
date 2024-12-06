#include "GameManager.h"

/**
 * @file GameManager.cpp
 * @brief ゲーム全体の管理（初期化、描画など）を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

void GameManager::Initialize()
{
	EngineCore::Initialize();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("GameTitleScene");
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

