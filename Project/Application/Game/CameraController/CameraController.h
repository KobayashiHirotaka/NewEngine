#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Application/GameObject/Character/Direction.h"

class CameraController
{
public:
	void Initialize();

	void Update(const Vector3 characterPosition1, const Vector3 characterPositon2, const Direction direction);

	void ImGui();

	Camera GetCamera() { return camera_; };

private:
	Input* input_ = nullptr;

	Camera camera_;

	Vector3 position_ = { 0.0f,1.0f,-10.5f };

	float cameraSpeed_ = 0.03f;

	float point_ = 2.0f;

	const float min_ = -10.5f;

	const float max_ = -13.5f;
};
