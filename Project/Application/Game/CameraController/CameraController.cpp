#include "CameraController.h"

void CameraController::Initialize()
{
	camera_.translation_ = position_;

	camera_.UpdateMatrix();
}

void CameraController::Update()
{
	camera_.UpdateMatrix();
}