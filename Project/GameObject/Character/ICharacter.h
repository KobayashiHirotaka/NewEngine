#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include <vector>

enum ModelParts
{
	kModelIndexBody,
	kModelIndexHead,
	kModelIndexL_arm,
	kModelIndexR_arm,
	kModelIndexHammer
};

class ICharacter
{
public:
	virtual void Initialize(const std::vector<Model*>& models);

	virtual void Update();

	virtual void Draw(const Camera& camera);

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:
	std::vector<Model*> models_;

	WorldTransform worldTransform_;
};
