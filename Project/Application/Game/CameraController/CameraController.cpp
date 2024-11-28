#include "CameraController.h"

void CameraController::Initialize()
{
	input_ = Input::GetInstance();

	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::Update()
{
#ifdef _ADJUSTMENT

	if (input_->PressKey(DIK_W))
	{
		position_.z += cameraSpeed_;
	}

	if (input_->PressKey(DIK_S))
	{
		position_.z -= cameraSpeed_;
	}

	if (input_->PressKey(DIK_D))
	{
		position_.x += cameraSpeed_;
	}

	if (input_->PressKey(DIK_A))
	{
		position_.x -= cameraSpeed_;
	}

#endif

	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::ImGui()
{
	ImGui::Begin("CameraController");
	ImGui::SliderFloat3("WTFT", &position_.x, -50.0f, 50.0f);
	ImGui::End();
}