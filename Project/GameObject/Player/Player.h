#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"

class Player
{
public:
	void Initialize(Model* model);

	void Update();

	void Draw(const Camera camera);

	Vector3 GetLocalPosition();

private:
	Model* model_ = nullptr;

	Input* input_ = nullptr;

	WorldTransform worldTransform_;

	Vector3 velocity_ = {};

	float destinationAngleY_ = 0.0f;
};

