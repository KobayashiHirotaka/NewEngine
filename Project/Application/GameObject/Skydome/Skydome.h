#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/IGame3dObject.h"

class Skydome
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera camera);

	void ImGui();

private:
	//input
	Input* input_ = nullptr;

	std::unique_ptr<Model> model_;

	WorldTransform worldTransform_;

	uint32_t animationIndex = 0;
};
