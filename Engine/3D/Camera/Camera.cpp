#include "Camera.h"

Camera::Camera() 
{
	//CBVの作成
	constBuff_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBuffDataViewProjection));
}

Camera::~Camera()
{

}

void Camera::UpdateViewMatrix()
{
	//カメラのワールド行列を作成
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotation_, translation_);

	//ビュー行列の計算
	matView_ = Inverse(worldMatrix);
}

void Camera::UpdateProjectionMatrix()
{
	//プロジェクション行列の計算
	matProjection_ = MakePerspectiveFovMatrix(fov_, aspectRatio_, nearClip_, farClip_);
}

void Camera::UpdateMatrix()
{
	//ビュー行列の計算
	Camera::UpdateViewMatrix();

	//プロジェクション行列の計算
	Camera::UpdateProjectionMatrix();

	//ビュープロジェクションを転送する
	Camera::TransferMatrix();
}

void Camera::TransferMatrix()
{
	//Resourceに書き込む
	ConstBuffDataViewProjection* viewProjectionData = nullptr;
	constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData));
	viewProjectionData->view = matView_;
	viewProjectionData->projection = matProjection_;
	constBuff_->Unmap(0, nullptr);
}