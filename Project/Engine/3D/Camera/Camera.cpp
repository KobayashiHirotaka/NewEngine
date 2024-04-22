#include "Camera.h"

Camera::Camera() 
{
	constBuff_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBuffDataCamera));
}

Camera::~Camera()
{

}

void Camera::UpdateMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	TransferMatrix();
}

void Camera::TransferMatrix()
{
	ConstBuffDataCamera* cameraData = nullptr;
	constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	cameraData->view = matView_;
	cameraData->projection = matProjection_;
	constBuff_->Unmap(0, nullptr);
}

void Camera::UpdateViewMatrix()
{
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_);
	matView_ = Inverse(cameraMatrix);
}

void Camera::UpdateProjectionMatrix()
{
	matProjection_ = MakePerspectiveFovMatrix(fov_, aspectRatio_, nearClip_, farClip_);
}