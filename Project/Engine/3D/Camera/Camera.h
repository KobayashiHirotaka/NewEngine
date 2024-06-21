#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <d3d12.h>
#include <wrl.h>

struct ConstBuffDataCamera
{
	Vector3 worldPosition;
	float padding;
	Matrix4x4 view;
	Matrix4x4 projection;
};

class Camera
{
public:
	Camera();

	~Camera();

	void UpdateViewMatrix();

	void UpdateProjectionMatrix();

	void UpdateMatrix();

	void TransferMatrix();

	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	//X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };

	//ローカル座標
	Vector3 translation_ = { 0.0f,25.0f,-100.0f };

	//視野角
	float fov_ = 0.45f;

	//アスペクト比
	float aspectRatio_ = (float)16 / 9;

	//近平面への距離（手前側）
	float nearClip_ = 0.1f;

	//遠平面への距離（奥側）
	float farClip_ = 300.0f;

	//ビュー行列
	Matrix4x4 matView_{};

	//プロジェクション行列
	Matrix4x4 matProjection_{};
};