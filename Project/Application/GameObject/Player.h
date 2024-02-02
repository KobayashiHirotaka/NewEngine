#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/IGame3dObject.h"

class Player : public IGame3dObject
{
public:
	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

private:
	Input* input_ = nullptr;

	const Camera* camera_ = nullptr;

	WorldTransform worldTransform_;
};

