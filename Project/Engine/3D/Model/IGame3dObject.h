#pragma once
#include "Engine//3D/Model/Model.h"

class Game3dObjectManager;

class IGame3dObject
{
public:
	virtual ~IGame3dObject() = default;

	virtual void Initialize();

	virtual void Update();

	virtual void Draw(Model* model, const Camera camera);

	const Model* GetModel() const { return model_; };

	void SetModel(Model* model) { model_ = model; };

	void SetPosition(const Vector3& position) { worldTransform_.translation = position; };

	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; };

	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; };

	void SetGameObjectManager(Game3dObjectManager* game3dObjectManager) { game3dObjectManager_ = game3dObjectManager; };

	const std::string GetTag() { return tag_; };

	void SetTag(const std::string tag) { tag_ = tag; };

private:
	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_{};

	Model* model_ = nullptr;

	std::string tag_;
};
