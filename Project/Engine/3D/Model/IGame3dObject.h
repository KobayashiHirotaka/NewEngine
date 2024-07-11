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

	//model
	const Model* GetModel() const { return model_.get(); }
	void SetModel(std::unique_ptr<Model> model) { model_ = std::move(model); }

	//gameObjectManagerのsetter
	void SetGameObjectManager(Game3dObjectManager* game3dObjectManager) { game3dObjectManager_ = game3dObjectManager; };

	//transformのsetter
	void SetPosition(const Vector3& position) { worldTransform_.translation = position; };
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; };
	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; };

	//characterDataのsetter
	void SetHP(const float& hp) { HP_ = hp; };
	void SetSpeed(const float& speed) { speed_ = speed; };

	//tag
	const std::string GetTag() { return tag_; };
	void SetTag(const std::string tag) { tag_ = tag; };

protected:
	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_{};

	std::unique_ptr<Model> model_ = nullptr;

	std::string tag_;

	float HP_;
	float speed_;
};
