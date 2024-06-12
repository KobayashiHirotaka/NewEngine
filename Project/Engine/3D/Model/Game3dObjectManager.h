#pragma once
#include "IGame3dObject.h"
#include "Game3dObjectFactory.h"
#include "Engine/3D/Camera/Camera.h"
#include <vector>
#include <memory>

class Game3dObjectManager
{
public:
	static Game3dObjectManager* GetInstance();

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	static IGame3dObject* CreateGameObject(const std::string& objectName);
	IGame3dObject* CreateGameObjectInternal(const std::string& objectName);

	Camera* CreateCameraObject(const std::string& objectName);
	Camera* CreateCameraObjectInternal(const std::string& objectName);

	template <typename Type>
	Type* CreateGameObjectFromType();

	template <typename Type>
	Type* CreateGameObjectInternalFromType();

	void SetGameObjectFactory(Game3dObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	template <typename Type>
	Type* GetGameObject(const std::string& tag);

private:
	Game3dObjectManager() = default;
	~Game3dObjectManager() = default;
	Game3dObjectManager(const Game3dObjectManager&) = delete;
	const Game3dObjectManager& operator=(const Game3dObjectManager&) = delete;

private:
	std::vector<std::unique_ptr<IGame3dObject>> gameObjects_{};

	std::vector<std::unique_ptr<Camera>> cameraObjects_{};

	Game3dObjectFactory* gameObjectFactory_;
};

template <typename Type>
Type* Game3dObjectManager::CreateGameObjectFromType()
{
	Type* gameObject = Game3dObjectManager::GetInstance()->CreateGameObjectInternalFromType<Type>();
	return gameObject;
}

template <typename Type>
Type* Game3dObjectManager::CreateGameObjectInternalFromType()
{
	Type* gameObject = new Type();
	gameObject->Initialize();
	gameObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<IGame3dObject>(gameObject));
	return gameObject;
}

template <typename Type>
Type* Game3dObjectManager::GetGameObject(const std::string& tag)
{
	for (std::unique_ptr<IGame3dObject>& gameObject : gameObjects_)
	{
		gameObject->GetTag();

		if (gameObject->GetTag() == tag)
		{
			return dynamic_cast<Type*> (gameObject.get());
		}
	}

	return nullptr;
}
