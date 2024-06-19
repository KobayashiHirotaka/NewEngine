#pragma once
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/IGame3dObject.h"

class Skydome : public IGame3dObject
{
public:
	void Initialize();

	void Update();

	void Draw(Model* model, const Camera camera);

	void ImGui();

private:
	WorldTransform worldTransform_;
};
