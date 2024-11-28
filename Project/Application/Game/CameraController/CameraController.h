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

	Camera& GetCamera() { return camera_; };

private:
	Input* input_ = nullptr;

	Camera camera_;

	Vector3 position_ = { 0.0f,0.8f,-5.0f };

	Vector3 cameraSpeed_ = { 0.25f, 0.5f, 0.01f };

	float point_ = 2.8f;

	float min_ = -5.0f;
	float max_ = -9.5f;

	float distance_ = 0.0f;
	float previousDistance_ = 0.0f;
};
