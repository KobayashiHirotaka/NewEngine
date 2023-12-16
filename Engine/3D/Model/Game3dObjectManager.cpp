#include "Game3dObjectManager.h"

Game3dObjectManager* Game3dObjectManager::GetInstance()
{
	static Game3dObjectManager instance;
	return &instance;
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
		gameObject->Draw(camera);
	}
}