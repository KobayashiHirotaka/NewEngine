#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/IGame3dObject.h"

class BackGround
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera camera);

	void ImGui();

private:
	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	WorldTransform worldTransform_;
};
