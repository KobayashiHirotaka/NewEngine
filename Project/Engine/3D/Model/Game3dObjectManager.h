#pragma once
#include "IGame3dObject.h"
#include <vector>
#include <memory>

class Game3dObjectManager
{
public:
	static Game3dObjectManager* GetInstance();

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

private:
	Game3dObjectManager() = default;
	~Game3dObjectManager() = default;
	Game3dObjectManager(const Game3dObjectManager&) = delete;
	const Game3dObjectManager& operator=(const Game3dObjectManager&) = delete;

private:
	std::vector<std::unique_ptr<IGame3dObject>> gameObjects_{};
};
