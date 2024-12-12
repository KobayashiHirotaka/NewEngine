/**
 * @file Camera.cpp
 * @brief カメラの更新、行列計算などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Camera.h"

Camera::Camera()
{
	//定数バッファを作成
	constBuff_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBuffDataCamera));
}

Camera::~Camera()
{

}

void Camera::UpdateMatrix()
{
	//ビュー行列の更新
	UpdateViewMatrix();

	//投影行列の更新
	UpdateProjectionMatrix();

	//行列の転送
	TransferMatrix();
}

void Camera::TransferMatrix()
{
	//定数バッファをマッピング
	ConstBuffDataCamera* cameraData = nullptr;
	constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	//カメラデータを設定
	cameraData->worldPosition = translation_;
	cameraData->view = matView_;
	cameraData->projection = matProjection_;

	//定数バッファをアンマップ
	constBuff_->Unmap(0, nullptr);
}

void Camera::UpdateViewMatrix()
{
	//アフィン変換行列を使用してビュー行列を計算
	Matrix4x4 cameraMatrix = MakeAffineMatrix(scale_, rotation_, translation_);
	matView_ = Inverse(cameraMatrix);
}

void Camera::UpdateProjectionMatrix()
{
	//視野角、アスペクト比、クリッピング平面を使って投影行列を計算
	matProjection_ = MakePerspectiveFovMatrix(fov_, aspectRatio_, nearClip_, farClip_);
}

void Camera::ImGui()
{
	//ImGui用の変数
	const float translationMin = -100.0f;
	const float translationMax = 100.0f;

	const float rotationMin = -0.0f;
	const float rotationMax = 16.0f;

	ImGui::Begin("Camera");
	ImGui::SliderFloat3("WTFT", &translation_.x, translationMin, translationMax);
	ImGui::SliderFloat3("WTFR", &rotation_.x, rotationMin, rotationMax);
	ImGui::End();
}