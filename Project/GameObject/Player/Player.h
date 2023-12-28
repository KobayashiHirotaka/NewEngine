#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"

class Player
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera camera);

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_;
};

