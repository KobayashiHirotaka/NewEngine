#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"

class CameraController
{
public:
	void Initialize();

	void Update();

	void Move(const Vector3 characterPosition1, const Vector3 characterPositon2);

	void ImGui();

	Camera GetCamera() { return camera_; };

private:
	Input* input_ = nullptr;

	Camera camera_;

	Vector3 position_ = { 0.0f,1.0f,-10.5f };

	const float cameraSpeed_ = 0.1f;
};
