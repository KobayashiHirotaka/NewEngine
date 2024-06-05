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

	//IGame3dObject* GetGameObject() { return gameObjects_.get(); };

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



