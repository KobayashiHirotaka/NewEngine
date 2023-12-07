#include "Camera.h"

void Camera::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();
	TransferMatrix();
}

void Camera::CreateConstBuffer()
{
	constBuff = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataViewProjection));
}

void Camera::Map()
{
	constBuff.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void Camera::UpdateMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	TransferMatrix();
}

void Camera::TransferMatrix()
{
	constMap->view = matView;
	constMap->projection = matProjection;
}

void Camera::UpdateViewMatrix()
{
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation, translation);
	matView = Inverse(cameraMatrix);
}

void Camera::UpdateProjectionMatrix()
{
	matProjection = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}