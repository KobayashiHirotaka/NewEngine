#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <d3d12.h>
#include <wrl.h>

struct ConstBuffDataViewProjection
{
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

	//CBV
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_ = nullptr;

	//角度
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };

	//座標
	Vector3 translation_ = { 0.0f,2.0f,-35.0f };

	//ビュー行列
	Matrix4x4 matView_{};

	//プロジェクション行列
	Matrix4x4 matProjection_{};

	//視野角
	float fov_ = 0.45f;

	//アスペクト比
	float aspectRatio_ = 1280.0f / 720.0f;

	//近平面への距離
	float nearClip_ = 0.1f;

	//遠平面への距離
	float farClip_ = 100.0f;
};