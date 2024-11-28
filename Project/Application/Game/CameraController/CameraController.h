#pragma once
#include "Engine/3D/Camera/Camera.h"

class CameraController
{
public:
	void Initialize();

	void Update();

	Camera GetCamera() { return camera_; };

private:
	Camera camera_;

	Vector3 position_ = { 0.0f,1.0f,-13.0f };
};
