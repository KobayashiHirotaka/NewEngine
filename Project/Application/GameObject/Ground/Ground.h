#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/IGame3dObject.h"

class Ground : public IGame3dObject
{
public:
	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

	void ImGui(const char* title)override;

private:
};