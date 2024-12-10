/**
 * @file Game3dObjectManager.cpp
 * @brief 3Dオブジェクトの管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Game3dObjectManager.h"

Game3dObjectManager* Game3dObjectManager::sInstance_ = nullptr;

Game3dObjectManager* Game3dObjectManager::GetInstance()
{
	if (sInstance_ == nullptr)
	{
		sInstance_ = new Game3dObjectManager();
	}
	return sInstance_;
}

void Game3dObjectManager::DeleteInstance()
{
	if (sInstance_ != nullptr)
	{
		delete sInstance_;
		sInstance_ = nullptr;
	}
}

void Game3dObjectManager::Initialize()
{
	gameObjects_.clear();
}

void Game3dObjectManager::Update()
{
	for (std::unique_ptr<IGame3dObject>& gameObject : gameObjects_)
	{
		gameObject->Update();
	}
}

void Game3dObjectManager::Draw(const Camera& camera)
{
	for (std::unique_ptr<IGame3dObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsVisible())
		{
			gameObject->Draw(camera);
		}
	}
}

IGame3dObject* Game3dObjectManager::CreateGameObject(const std::string& objectName)
{
	IGame3dObject* gameObject = Game3dObjectManager::GetInstance()->CreateGameObjectInternal(objectName);
	return gameObject;
}

IGame3dObject* Game3dObjectManager::CreateGameObjectInternal(const std::string& objectName)
{
	assert(gameObjectFactory_);
	IGame3dObject* gameObject = gameObjectFactory_->CreateGameObject(objectName);
	gameObject->Initialize();
	gameObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<IGame3dObject>(gameObject));
	return gameObject;
}




