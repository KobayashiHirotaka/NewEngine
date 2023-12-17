#pragma once
#include "Engine//3D/Model/Model.h"

class Game3dObjectManager;

class IGame3dObject
{
public:
	virtual ~IGame3dObject() = default;

	virtual void Initialize();

	virtual void Update();

	virtual void Draw(const Camera& camera);

	const Model* GetModel() const { return model_; };

	void SetModel(Model* model) { model_ = model; };

	void SetGameObjectManager(Game3dObjectManager* game3dObjectManager) { game3dObjectManager_ = game3dObjectManager; };

private:

	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_{};

	Model* model_ = nullptr;
};

