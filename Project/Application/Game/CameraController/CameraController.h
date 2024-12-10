/**
 * @file CameraController.h
 * @brief ゲーム中のカメラの制御、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Application/GameObject/Character/Direction.h"

class CameraController
{
public:
	void Initialize();

	void Update(const Vector3 characterPosition1, const Vector3 characterPositon2);

	void ImGui();

	Camera& GetCamera() { return camera_; };

	Vector2 GetCenter() { return center_; };

private:
	Input* input_ = nullptr;

	Camera camera_;

	Vector3 position_ = { 0.0f,0.8f,-5.0f };

	Vector3 cameraSpeed_ = { 1.5f, 0.5f, 0.03f };

	Vector2 center_ = { 0.0f,0.0f };

	const float kPoint_ = 2.8f;

	const float kMin_ = -5.0f;
	const float kMax_ = -8.0f;

	float distance_ = 0.0f;
	float previousDistance_ = 0.0f;
};
